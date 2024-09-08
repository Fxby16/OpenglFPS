#pragma once

#include <Mesh.hpp>
#include <Texture.hpp>

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <deque>
#include <mutex>
#include <assimp/scene.h>
#include <glm.hpp>

extern glm::mat4 g_DummyTransform;

struct BoneInfo{
    int id;
    glm::mat4 offset;
};

class Model{
public:
    Model() = default;
    ~Model() = default;

    void Load(const std::string& path, bool gamma = false);
    void Load(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false);
    void Unload();

    void SetMeshes(const std::vector<Mesh>& meshes);
    void SetTransforms(const std::vector<glm::mat4>& transforms);
    void SetTransform(uint32_t index, const glm::mat4& transform);

    void AddTransform(const glm::mat4& transform);
    void RemoveTransform(uint32_t index);

    void Draw(Shader& shader, glm::mat4 view, glm::mat4 model);
    void DrawShadows(Shader& shader, glm::mat4 light_space_matrix, glm::mat4 model);

    inline std::vector<Mesh>& GetMeshes() { return m_Meshes; }
    inline std::vector<glm::mat4>& GetTransforms() { return m_Transforms; }
    inline glm::mat4& GetTransform(uint32_t index) { return (index < m_Transforms.size()) ? m_Transforms[index] : g_DummyTransform; }
    inline void ClearTransforms() { m_Transforms.clear(); }
    inline const std::string& GetDirectory() const { return m_Directory; }
    inline bool GetGammaCorrection() { return m_GammaCorrection; }
    inline const std::string& GetPath() const { return m_Path; }
    inline const std::string& GetName() const { return m_Name; }
    inline const std::map<std::string, BoneInfo>& GetBoneInfoMap() const { return m_BoneInfoMap; }
    inline int GetBoneCount() const { return m_BoneCount; }
    inline void IncrementBoneCount() { m_BoneCount++; }
    inline std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
    inline void SetBoneInfoMap(const std::map<std::string, BoneInfo>& bone_info_map) { m_BoneInfoMap = bone_info_map; }

    void ResetVertexBoneData(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int bone_id, float weight);
    void ExtractBoneWeightForVertices(aiMesh* mesh, const aiScene* scene, std::vector<Vertex>& vertices);

private:
    void ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parent_transform = glm::mat4(1.0f));
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 parent_transform = glm::mat4(1.0f));
    std::vector<uint32_t> LoadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, const std::string& typeName);

    std::vector<Mesh> m_Meshes;
    std::vector<glm::mat4> m_Transforms;
    std::unordered_map<std::string, uint32_t> m_LoadedTextures;
    std::string m_Directory;

    std::string m_Path;
    std::string m_Name;
    
    bool m_GammaCorrection;

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCount = 0;
};

void SetLogsOutput(std::deque<std::string>* logs, std::mutex* logs_mutex);
void UnsetLogsOutput();