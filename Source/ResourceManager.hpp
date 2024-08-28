#pragma once

#include <unordered_map>
#include <set>

#include <Model.hpp>
#include <Texture.hpp>
#include <Random.hpp>
#include <Shader.hpp>

extern uint32_t g_Cube;
extern uint32_t g_Sphere;

class ResourceManager{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void Init();
    void Deinit();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    inline Model& GetModel(uint32_t id) { return m_Models[id]; }
    inline Texture& GetTexture(uint32_t id) { return m_Textures[id]; }
    inline Shader& GetShader(uint32_t id) { return m_Shaders[id]; }

    uint32_t LoadModel(const std::string& path, bool gamma = false);
    uint32_t LoadModel(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false);
    uint32_t LoadTexture(const std::string& path, bool flip = true);
    uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);
    uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path);

    void UnloadModel(uint32_t id);
    void UnloadTexture(uint32_t id);
    void UnloadShader(uint32_t id);

    void UnloadModelsWithoutTransforms();

    inline std::unordered_map<uint32_t, Model>& GetModels() { return m_Models; }
    inline std::unordered_map<uint32_t, Texture>& GetTextures() { return m_Textures; }
    inline std::unordered_map<uint32_t, Shader>& GetShaders() { return m_Shaders; }
    inline unsigned int GetShadowMapArray() const { return m_ShadowMapArray; }
    inline unsigned int GetCubeShadowMapArray() const { return m_CubeShadowMapArray; }

    void HotReloadShaders();
    void DrawModels(Shader& shader, glm::mat4 view);
    void DrawModelsShadows(Shader& shader, glm::mat4 light_space_matrix);

    int GetShadowMapArrayIndex();
    int GetCubeShadowMapArrayIndex();
    void FreeShadowMapArrayIndex(int index);
    void FreeCubeShadowMapArrayIndex(int index);

private:

    std::unordered_map<uint32_t, Model> m_Models;
    std::unordered_map<uint32_t, Texture> m_Textures;
    std::unordered_map<uint32_t, Shader> m_Shaders;

    unsigned int m_ShadowMapArray, m_CubeShadowMapArray;
    std::unordered_set<int> m_ShadowMapArrayIndices, m_CubeShadowMapArrayIndices;
};

extern void InitResourceManager();
extern void DeinitResourceManager();
extern ResourceManager& GetResourceManager();

inline Model& GetModel(uint32_t id) { return GetResourceManager().GetModel(id); }
inline Texture& GetTexture(uint32_t id) { return GetResourceManager().GetTexture(id); }
inline Shader& GetShader(uint32_t id) { return GetResourceManager().GetShader(id); }

extern uint32_t g_GBufferShader, g_DeferredShader, g_ShadowMapShader, g_PointLightShadowMapShader;
inline Shader& GetGBufferShader() { return GetShader(g_GBufferShader); }
inline Shader& GetDeferredShader() { return GetShader(g_DeferredShader); }
inline Shader& GetShadowMapShader() { return GetShader(g_ShadowMapShader); }
inline Shader& GetPointLightShadowMapShader() { return GetShader(g_PointLightShadowMapShader); }

extern uint32_t LoadModel(const std::string& path, bool gamma = false);
extern uint32_t LoadModel(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false);
extern uint32_t LoadTexture(const std::string& path, bool flip = true);
extern uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);
extern uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path);

extern void UnloadModel(uint32_t id);
extern void UnloadTexture(uint32_t id);
extern void UnloadShader(uint32_t id);

extern void UnloadModelsWithoutTransforms();

inline std::unordered_map<uint32_t, Model>& GetModels() { return GetResourceManager().GetModels(); }
inline std::unordered_map<uint32_t, Texture>& GetTextures() { return GetResourceManager().GetTextures(); }
inline std::unordered_map<uint32_t, Shader>& GetShaders() { return GetResourceManager().GetShaders(); }
inline unsigned int GetShadowMapArray() { return GetResourceManager().GetShadowMapArray(); }
inline unsigned int GetCubeShadowMapArray() { return GetResourceManager().GetCubeShadowMapArray(); }

extern void HotReloadShaders();
extern void ClearModels();
extern void DrawModels(Shader& shader, glm::mat4 view);
extern void DrawModelsShadows(Shader& shader, glm::mat4 light_space_matrix);

inline int GetShadowMapArrayIndex() { return GetResourceManager().GetShadowMapArrayIndex(); }
inline int GetCubeShadowMapArrayIndex() { return GetResourceManager().GetCubeShadowMapArrayIndex(); }
inline void FreeShadowMapArrayIndex(int index) { GetResourceManager().FreeShadowMapArrayIndex(index); }
inline void FreeCubeShadowMapArrayIndex(int index) { GetResourceManager().FreeCubeShadowMapArrayIndex(index); }