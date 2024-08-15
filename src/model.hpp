#pragma once

#include <mesh.hpp>
#include <texture.hpp>

#include <vector>
#include <unordered_map>
#include <string>
#include <deque>
#include <mutex>
#include <assimp/scene.h>
#include <glm.hpp>

class Model{
public:
    Model() = default;
    ~Model() = default;

    void Load(const std::string& path, bool is_compressed = true, bool pbr = true, bool gamma = false);
    void Load(const std::vector<Mesh>& meshes, bool is_compressed = true, bool pbr = true, bool gamma = false);
    void Unload();

    void SetMeshes(const std::vector<Mesh>& meshes);

    void Draw(Shader& shader, glm::mat4 view, glm::mat4 model);

    inline std::vector<Mesh>& GetMeshes() { return m_Meshes; }
    inline const std::string& GetDirectory() const { return m_Directory; }
    inline bool GetGammaCorrection() { return m_GammaCorrection; }

private:
    void ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parent_transform = glm::mat4(1.0f));
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 parent_transform = glm::mat4(1.0f));
    std::vector<uint32_t> LoadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, const std::string& typeName);

    std::vector<Mesh> m_Meshes;
    std::string m_Directory;
    
    bool m_GammaCorrection;
    bool m_PBREnabled;
    bool m_Compressed;
};

void SetLogsOutput(std::deque<std::string>* logs, std::mutex* logs_mutex);
void UnsetLogsOutput();