#include <model.hpp>
#include <frustum.hpp>
#include <math.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <cstdio>
#include <limits>
#include <stack>

Matrix AiToRaylib(const aiMatrix4x4& from)
{
    Matrix to;

    to.m0 = from.a1; to.m4 = from.a2; to.m8 = from.a3; to.m12 = from.a4;
    to.m1 = from.b1; to.m5 = from.b2; to.m9 = from.b3; to.m13 = from.b4;
    to.m2 = from.c1; to.m6 = from.c2; to.m10 = from.c3; to.m14 = from.c4;
    to.m3 = from.d1; to.m7 = from.d2; to.m11 = from.d3; to.m15 = from.d4;

    return to;
}

void ModelEx::Load(const std::string& path, bool pbr, bool gamma)
{
    m_PBREnabled = pbr;
    m_GammaCorrection = gamma;

    printf("Loading model %s\n", path.c_str());

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | 
        aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        fprintf(stderr, "ERROR::ASSIMP::%s\n", importer.GetErrorString());
        return;
    }

    m_Directory = path;
    m_Directory = m_Directory.substr(0, m_Directory.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
}

void ModelEx::Unload()
{
    for(auto &[tex_name, tex] : m_TexturesLoaded){
        tex.Free();
    }

    for(MeshEx& mesh : m_Meshes){
        mesh.Free();
    }
}

void ModelEx::Draw(ShaderEx& shader, Matrix view, Matrix model)
{
    for(int i = 0; i < m_Meshes.size(); i++){
        m_Meshes[i].Draw(shader, view, model, m_PBREnabled);
    }
}

void ModelEx::ProcessNode(aiNode* node, const aiScene* scene, Matrix parent_transform)
{
    Matrix transform = MatrixMultiply(parent_transform, AiToRaylib(node->mTransformation));

    for(int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene, transform));
    }

    //simulate recursion
    std::stack<aiNode*> stack;

    for(int i = 0; i < node->mNumChildren; i++){
        stack.push(node->mChildren[i]);
    }

    while(!stack.empty()){
        aiNode* current = stack.top();
        stack.pop();

        for(int i = 0; i < current->mNumMeshes; i++){
            aiMesh* mesh = scene->mMeshes[current->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene, transform));
        }

        for(int i = 0; i < current->mNumChildren; i++){
            stack.push(current->mChildren[i]);
        }
    }
}

MeshEx ModelEx::ProcessMesh(aiMesh* mesh, const aiScene* scene, Matrix parent_transform)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureEx> textures;

    Vector3 aabb_min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    Vector3 aabb_max = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};

    for(int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;
        Vector3 vector;

        if(mesh->HasPositions()){
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
        
            vertex.Position = MatVecMul(parent_transform, vector);
            aabb_min = Vector3Min(aabb_min, vertex.Position);
            aabb_max = Vector3Max(aabb_max, vertex.Position);
        }

        if(mesh->HasNormals()){
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = MatVecMul(parent_transform, vector);
        }

        if(mesh->HasTextureCoords(0)){
            Vector2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }else{
            vertex.TexCoords = (Vector2){0.0f, 0.0f};
        }

        if(mesh->HasTangentsAndBitangents()){
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
        
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }else{
            vertex.Tangent = (Vector3){0.0f, 0.0f, 0.0f};
            vertex.Bitangent = (Vector3){0.0f, 0.0f, 0.0f};
        }

        vertices.push_back(vertex);
    }

    for(int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    if(m_PBREnabled){
        // Load PBR textures
        std::vector<TextureEx> pbrTextures;

        // Albedo (diffuse) map
        std::vector<TextureEx> albedoMaps = LoadMaterialTextures(material, scene, aiTextureType_BASE_COLOR, "albedoMap");
        pbrTextures.insert(pbrTextures.end(), albedoMaps.begin(), albedoMaps.end());

        // Normal map
        std::vector<TextureEx> normalMaps = LoadMaterialTextures(material, scene, aiTextureType_NORMALS, "normalMap");
        pbrTextures.insert(pbrTextures.end(), normalMaps.begin(), normalMaps.end());

        // Roughness map
        std::vector<TextureEx> roughnessMaps = LoadMaterialTextures(material, scene, aiTextureType_DIFFUSE_ROUGHNESS, "roughnessMap");
        pbrTextures.insert(pbrTextures.end(), roughnessMaps.begin(), roughnessMaps.end());

        // Metallic map
        std::vector<TextureEx> metallicMaps = LoadMaterialTextures(material, scene, aiTextureType_METALNESS, "metallicMap");
        pbrTextures.insert(pbrTextures.end(), metallicMaps.begin(), metallicMaps.end());

        // AO (ambient occlusion) map
        std::vector<TextureEx> aoMaps = LoadMaterialTextures(material, scene, aiTextureType_AMBIENT_OCCLUSION, "aoMap");
        pbrTextures.insert(pbrTextures.end(), aoMaps.begin(), aoMaps.end());

        textures.insert(textures.end(), pbrTextures.begin(), pbrTextures.end());
    }else{
        //diffuse maps
        std::vector<TextureEx> diffuseMaps = LoadMaterialTextures(material, scene, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        //specular maps
        //std::vector<TextureEx> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        //normal maps
        //std::vector<TextureEx> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        //height maps
        //std::vector<TextureEx> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    return MeshEx(vertices, indices, textures, (BoundingBox){aabb_min, aabb_max});
}

std::vector<TextureEx> ModelEx::LoadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, const std::string& typeName)
{
    std::vector<TextureEx> textures;

    for(int i = 0; i < mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);

        printf("Texture type: %s\n", typeName.c_str());

        for(int i = 0; i < strlen(str.C_Str()); i++){
            if(str.data[i] == '\\'){
                str.data[i] = '/';
            }
        }

        bool skip = false;
        if(m_TexturesLoaded.find(str.C_Str()) != m_TexturesLoaded.end()){
            textures.push_back(m_TexturesLoaded[str.C_Str()]);
            skip = true;
            printf("Reusing texture %s\n", str.C_Str());
        }

        if(!skip){
            if(str.data[0] != '*'){
                TextureEx texture(m_Directory + "/" + str.C_Str(), typeName, false);
                textures.push_back(texture);
                m_TexturesLoaded[str.C_Str()] = texture;
            }else{
                //int textureIndex = std::atoi(str.C_Str() + 1);
                //if (textureIndex >= 0 && textureIndex < scene->mNumTextures) {
                //    aiTexture* embeddedTexture = scene->mTextures[textureIndex];
                //    // Create a Texture object from the embedded texture data
                //    Texture texture(embeddedTexture->mFilename.C_Str(), (unsigned char*)embeddedTexture->pcData, typeName, embeddedTexture->mWidth, embeddedTexture->mHeight, false);
                //    textures.push_back(texture);
                //    m_TexturesLoaded.push_back(texture);
                //}

                printf("Embedded textures not supported\n");
            }
        }
    }

    return textures;
}