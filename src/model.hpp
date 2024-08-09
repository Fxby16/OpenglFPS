#pragma once

#include <mesh.hpp>
#include <texture.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <assimp/scene.h>
#include <glm.hpp>

class Model{
public:
    Model() = default;
    ~Model() = default;

    void Load(const std::string& path, bool pbr = true, bool gamma = false);
    void Unload();

    void Draw(Shader& shader, glm::mat4 view, glm::mat4 model);

    inline std::vector<Mesh>& GetMeshes() { return m_Meshes; }
    inline std::unordered_map<std::string, Texture>& GetTexturesLoaded() { return m_TexturesLoaded; }
    inline std::string& GetDirectory() { return m_Directory; }
    inline bool GetGammaCorrection() { return m_GammaCorrection; }

private:
    void ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parent_transform = glm::mat4(1.0f));
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 parent_transform = glm::mat4(1.0f));
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, const std::string& typeName);

    std::vector<Mesh> m_Meshes;
    std::unordered_map<std::string, Texture> m_TexturesLoaded;
    std::string m_Directory;
    
    bool m_GammaCorrection;
    bool m_PBREnabled;
};