#include <Model.hpp>
#include <Frustum.hpp>
#include <Log.hpp>
#include <ResourceManager.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm.hpp>

#include <cstdio>
#include <limits>
#include <stack>

static std::deque<std::string>* g_Logs = nullptr;
static std::mutex* g_LogsMutex = nullptr;

glm::mat4 g_DummyTransform = glm::mat4(1.0f);

glm::mat4 AiToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    return to;
}

void Model::Load(const std::string& path, bool gamma)
{
    m_Path = path;
    m_GammaCorrection = gamma;

    if(!g_Logs){
        LogMessage("Loading model %s\n", path.c_str());
    }else{
        g_LogsMutex->lock();
        g_Logs->push_back("Loading model " + path + "\n");
        g_LogsMutex->unlock();
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace |
                                                   aiProcess_Triangulate |
                                                   aiProcess_JoinIdenticalVertices |
                                                   aiProcess_FlipUVs |
                                                   aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        LogError("ASSIMP::%s\n", importer.GetErrorString());
        return;
    }

    m_Directory = path;
    m_Directory = m_Directory.substr(0, m_Directory.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
}

void Model::Load(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma)
{
    m_Name = model_name;
    m_GammaCorrection = gamma;

    m_Meshes = meshes;
}

void Model::Unload()
{
    LogMessage("Unloading model");

    for(Mesh& mesh : m_Meshes){
        mesh.Free();
    }

    m_Meshes.clear();
    m_Transforms.clear();
    m_LoadedTextures.clear();
}

void Model::SetMeshes(const std::vector<Mesh>& meshes)
{
    m_Meshes = meshes;
}

void Model::SetTransforms(const std::vector<glm::mat4>& transforms)
{
    m_Transforms = transforms;
}

void Model::SetTransform(uint32_t index, const glm::mat4& transform)
{
    while(m_Transforms.size() <= index){
    
    }

    m_Transforms[index] = transform;
}

void Model::AddTransform(const glm::mat4& transform)
{
    m_Transforms.push_back(transform);
}

void Model::RemoveTransform(uint32_t index)
{
    if(index < m_Transforms.size()){
        m_Transforms.erase(m_Transforms.begin() + index);
    }
}

void Model::Draw(Shader& shader, glm::mat4 view, glm::mat4 model)
{
    for(int i = 0; i < m_Meshes.size(); i++){
        m_Meshes[i].Draw(shader, view, model);
    }
}

void Model::DrawShadows(Shader& shader, glm::mat4 light_space_matrix, glm::mat4 model)
{
    for(int i = 0; i < m_Meshes.size(); i++){
        m_Meshes[i].DrawShadows(shader, light_space_matrix, model);
    }
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parent_transform)
{
    glm::mat4 transform = parent_transform * AiToGlm(node->mTransformation);

    for(int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene, transform));
    }

    //simulate recursion
    std::stack<std::pair<aiNode*, glm::mat4>> stack;

    for(int i = 0; i < node->mNumChildren; i++){
        stack.push({node->mChildren[i], transform});
    }

    while(!stack.empty()){
        aiNode* current_node = stack.top().first;
        glm::mat4 current_transform = stack.top().second;
        stack.pop();

        current_transform = current_transform * AiToGlm(current_node->mTransformation);

        for(int i = 0; i < current_node->mNumMeshes; i++){
            aiMesh* mesh = scene->mMeshes[current_node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene, current_transform));
        }

        for(int i = 0; i < current_node->mNumChildren; i++){
            stack.push({current_node->mChildren[i], current_transform});
        }
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 parent_transform)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<uint32_t> textures;

    glm::vec3 aabb_min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    glm::vec3 aabb_max = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};

    for(int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;
        glm::vec3 vector;

        if(mesh->HasPositions()){
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
        
            vertex.Position = parent_transform * glm::vec4(vector, 1.0f);
            aabb_min.x = glm::min(aabb_min.x, vertex.Position.x);
            aabb_min.y = glm::min(aabb_min.y, vertex.Position.y);
            aabb_min.z = glm::min(aabb_min.z, vertex.Position.z);
            aabb_max.x = glm::max(aabb_max.x, vertex.Position.x);
            aabb_max.y = glm::max(aabb_max.y, vertex.Position.y);
            aabb_max.z = glm::max(aabb_max.z, vertex.Position.z);
        }

        if(mesh->HasNormals()){
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = parent_transform * glm::vec4(vector, 0.0f);
        }

        if(mesh->HasTextureCoords(0)){
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }else{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
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
            vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
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

    Mesh mesh_obj(vertices, indices, AABB(aabb_min, aabb_max));

    std::vector<uint32_t> pbrTextures;

    std::vector<uint32_t> albedoMaps = LoadMaterialTextures(material, scene, aiTextureType_BASE_COLOR, "albedoMap");
    pbrTextures.insert(pbrTextures.end(), albedoMaps.begin(), albedoMaps.end());
    if(albedoMaps.size() > 0){
        mesh_obj.SetHasTexture(ALBEDO, true);
    }else{
        albedoMaps = LoadMaterialTextures(material, scene, aiTextureType_DIFFUSE, "albedoMap");
        pbrTextures.insert(pbrTextures.end(), albedoMaps.begin(), albedoMaps.end());
        if(albedoMaps.size() > 0){
            mesh_obj.SetHasTexture(ALBEDO, true);
        }
    }

    std::vector<uint32_t> normalMaps = LoadMaterialTextures(material, scene, aiTextureType_NORMALS, "normalMap");
    pbrTextures.insert(pbrTextures.end(), normalMaps.begin(), normalMaps.end());
    if(normalMaps.size() > 0){
        mesh_obj.SetHasTexture(NORMAL, true);
    }

    std::vector<uint32_t> roughnessMaps = LoadMaterialTextures(material, scene, aiTextureType_DIFFUSE_ROUGHNESS, "roughnessMap");
    pbrTextures.insert(pbrTextures.end(), roughnessMaps.begin(), roughnessMaps.end());
    if(roughnessMaps.size() > 0){
        mesh_obj.SetHasTexture(ROUGHNESS, true);
    }

    std::vector<uint32_t> metallicMaps = LoadMaterialTextures(material, scene, aiTextureType_METALNESS, "metallicMap");
    pbrTextures.insert(pbrTextures.end(), metallicMaps.begin(), metallicMaps.end());
    if(metallicMaps.size() > 0){
        mesh_obj.SetHasTexture(METALLIC, true);
    }

    std::vector<uint32_t> aoMaps = LoadMaterialTextures(material, scene, aiTextureType_AMBIENT_OCCLUSION, "aoMap");
    pbrTextures.insert(pbrTextures.end(), aoMaps.begin(), aoMaps.end());
    if(aoMaps.size() > 0){
        mesh_obj.SetHasTexture(AO, true);
    }

    textures.insert(textures.end(), pbrTextures.begin(), pbrTextures.end());

    mesh_obj.SetTextures(textures);

    return mesh_obj;
}

std::vector<uint32_t> Model::LoadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, const std::string& typeName)
{
    std::vector<uint32_t> textures;

    for(int i = 0; i < mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);

        LogMessage("Texture type: %s\n", typeName.c_str());

        for(int i = 0; i < strlen(str.C_Str()); i++){
            if(str.data[i] == '\\'){
                str.data[i] = '/';
            }
        }

        if(str.data[0] != '*'){
            if(!g_Logs){
                LogMessage("Loading texture %s\n", str.C_Str());
            }else{
                g_LogsMutex->lock();
                g_Logs->push_back("Loading texture " + std::string(str.C_Str()) + "\n");
                g_LogsMutex->unlock();
            }

            uint32_t texture;
            if(m_LoadedTextures.find(str.C_Str()) != m_LoadedTextures.end()){
                texture = m_LoadedTextures[str.C_Str()];
            }else{
                texture = LoadTexture(m_Directory + "/" + str.C_Str(), typeName, false);
                m_LoadedTextures[str.C_Str()] = texture;
            }

            textures.push_back(texture);
        }else{
            //int textureIndex = std::atoi(str.C_Str() + 1);
            //if (textureIndex >= 0 && textureIndex < scene->mNumTextures) {
            //    aiTexture* embeddedTexture = scene->mTextures[textureIndex];
            //    // Create a Texture object from the embedded texture data
            //    Texture texture(embeddedTexture->mFilename.C_Str(), (unsigned char*)embeddedTexture->pcData, typeName, embeddedTexture->mWidth, embeddedTexture->mHeight, false);
            //    textures.push_back(texture);
            //    m_TexturesLoaded.push_back(texture);
            //}

            LogMessage("Embedded textures not supported\n");
        }
    }

    return textures;
}

void SetLogsOutput(std::deque<std::string>* logs, std::mutex* logs_mutex)
{
    g_Logs = logs;
    g_LogsMutex = logs_mutex;
}

void UnsetLogsOutput()
{
    g_Logs = nullptr;
    g_LogsMutex = nullptr;
}