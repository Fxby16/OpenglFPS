#pragma once

#include <mesh.hpp>
#include <texture.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <assimp/scene.h>
#include <raylib.h>
#include <raymath.h>

class ModelEx{
public:
    ModelEx() = default;
    ~ModelEx() = default;

    void Load(const std::string& path, bool pbr = true, bool gamma = false);
    void Unload();

    void Draw(ShaderEx& shader, Matrix view, Matrix model);

    inline std::vector<MeshEx>& GetMeshes() { return m_Meshes; }
    inline std::unordered_map<std::string, TextureEx>& GetTexturesLoaded() { return m_TexturesLoaded; }
    inline std::string& GetDirectory() { return m_Directory; }
    inline bool GetGammaCorrection() { return m_GammaCorrection; }

private:
    void ProcessNode(aiNode* node, const aiScene* scene, Matrix parent_transform = MatrixIdentity());
    MeshEx ProcessMesh(aiMesh* mesh, const aiScene* scene, Matrix parent_transform = MatrixIdentity());
    std::vector<TextureEx> LoadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, const std::string& typeName);

    std::vector<MeshEx> m_Meshes;
    std::unordered_map<std::string, TextureEx> m_TexturesLoaded;
    std::string m_Directory;
    
    bool m_GammaCorrection;
    bool m_PBREnabled;
};