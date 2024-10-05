#pragma once

#include <unordered_map>
#include <set>

#include <Model.hpp>
#include <Texture.hpp>
#include <Random.hpp>
#include <Shader.hpp>
#include <Animator.hpp>
#include <Lights.hpp>

extern uint32_t g_Cube;
extern uint32_t g_Sphere;

struct SkinnedModel{
    Model model;
    Animator animator;

    SkinnedModel() = default;
    SkinnedModel(const Model& model, const Animator& animator) : model(model), animator(animator) {}

    void AddAnimation(const std::string& animationPath, float ticksPerSecond = 0.0f){
        animator.AddAnimation(animationPath, model, ticksPerSecond);
    }
};

class ResourceManager{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void Init();
    void Deinit();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    inline Model* GetModel(uint32_t id) { return m_Models.find(id) != m_Models.end() ? &m_Models[id] : nullptr; }
    inline SkinnedModel* GetSkinnedModel(uint32_t id) { return m_SkinnedModels.find(id) != m_SkinnedModels.end() ? &m_SkinnedModels[id] : nullptr; }
    inline Texture* GetTexture(uint32_t id) { return m_Textures.find(id) != m_Textures.end() ? &m_Textures[id] : nullptr; }
    inline Shader* GetShader(uint32_t id) { return m_Shaders.find(id) != m_Shaders.end() ? &m_Shaders[id] : nullptr; }
    inline DirectionalLight* GetDirectionalLight(uint32_t id) { return m_DirectionalLights.find(id) != m_DirectionalLights.end() ? &m_DirectionalLights[id] : nullptr; }
    inline PointLight* GetPointLight(uint32_t id) { return m_PointLights.find(id) != m_PointLights.end() ? &m_PointLights[id] : nullptr; }
    inline SpotLight* GetSpotLight(uint32_t id) { return m_SpotLights.find(id) != m_SpotLights.end() ? &m_SpotLights[id] : nullptr; }

    uint32_t LoadModel(const std::string& path, bool gamma = false);
    uint32_t LoadModel(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false);
    void LoadModel(uint32_t id, const std::string& path, bool gamma = false);
    void LoadModel(uint32_t id, const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false);
    uint32_t LoadSkinnedModel(const std::string& path, const std::string& animationPath, float ticksPerSecond = 0.0f, bool gamma = false);
    void LoadSkinnedModel(uint32_t id, const std::string& path, const std::string& animationPath, float ticksPerSecond = 0.0f, bool gamma = false);
    uint32_t LoadTexture(const std::string& path, bool flip = true);
    uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);
    uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path);
    uint32_t LoadDirectionalLight(const glm::vec3& direction, const glm::vec3& color);
    uint32_t LoadPointLight(const glm::vec3& position, const glm::vec3& color);
    uint32_t LoadSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float cutOff, float outerCutOff);

    void UnloadModel(uint32_t id);
    void UnloadSkinnedModel(uint32_t id);
    void UnloadTexture(uint32_t id);
    void UnloadShader(uint32_t id);
    void UnloadDirectionalLight(uint32_t id);
    void UnloadPointLight(uint32_t id);
    void UnloadSpotLight(uint32_t id);

    void UnloadModelsWithoutTransforms();

    inline std::unordered_map<uint32_t, Model>& GetModels() { return m_Models; }
    inline std::unordered_map<uint32_t, SkinnedModel>& GetSkinnedModels() { return m_SkinnedModels; }
    inline std::unordered_map<uint32_t, Texture>& GetTextures() { return m_Textures; }
    inline std::unordered_map<uint32_t, Shader>& GetShaders() { return m_Shaders; }
    inline std::unordered_map<uint32_t, DirectionalLight>& GetDirectionalLights() { return m_DirectionalLights; }
    inline std::unordered_map<uint32_t, PointLight>& GetPointLights() { return m_PointLights; }
    inline std::unordered_map<uint32_t, SpotLight>& GetSpotLights() { return m_SpotLights; }
    inline unsigned int GetShadowMapArray() const { return m_ShadowMapArray; }
    inline unsigned int GetCubeShadowMapArray() const { return m_CubeShadowMapArray; }

    void HotReloadShaders();
    void DrawModels(Shader& shader, glm::mat4 view);
    void DrawModelsShadows(Shader& shader, glm::mat4 light_space_matrix);
    void DrawShadowMaps();
    void SetShadowMaps();

    int GetShadowMapArrayIndex();
    int GetCubeShadowMapArrayIndex();
    void FreeShadowMapArrayIndex(int index);
    void FreeCubeShadowMapArrayIndex(int index);

    void UpdateAnimations(float deltaTime);

private:

    std::unordered_map<uint32_t, Model> m_Models;
    std::unordered_map<uint32_t, SkinnedModel> m_SkinnedModels;
    std::unordered_map<uint32_t, Texture> m_Textures;
    std::unordered_map<uint32_t, Shader> m_Shaders;
    std::unordered_map<uint32_t, DirectionalLight> m_DirectionalLights;
    std::unordered_map<uint32_t, PointLight> m_PointLights;
    std::unordered_map<uint32_t, SpotLight> m_SpotLights;

    unsigned int m_ShadowMapArray, m_CubeShadowMapArray;
    std::unordered_set<int> m_ShadowMapArrayIndices, m_CubeShadowMapArrayIndices;
};

extern void InitResourceManager();
extern void DeinitResourceManager();
extern ResourceManager& GetResourceManager();

inline Model* GetModel(uint32_t id) { return GetResourceManager().GetModel(id); }
inline SkinnedModel* GetSkinnedModel(uint32_t id) { return GetResourceManager().GetSkinnedModel(id); }
inline Texture* GetTexture(uint32_t id) { return GetResourceManager().GetTexture(id); }
inline Shader* GetShader(uint32_t id) { return GetResourceManager().GetShader(id); }
inline DirectionalLight* GetDirectionalLight(uint32_t id) { return GetResourceManager().GetDirectionalLight(id); }
inline PointLight* GetPointLight(uint32_t id) { return GetResourceManager().GetPointLight(id); }
inline SpotLight* GetSpotLight(uint32_t id) { return GetResourceManager().GetSpotLight(id); }

extern uint32_t g_GBufferShader, g_DeferredShader, g_ShadowMapShader, g_PointLightShadowMapShader;
inline Shader& GetGBufferShader() { return *GetShader(g_GBufferShader); }
inline Shader& GetDeferredShader() { return *GetShader(g_DeferredShader); }
inline Shader& GetShadowMapShader() { return *GetShader(g_ShadowMapShader); }
inline Shader& GetPointLightShadowMapShader() { return *GetShader(g_PointLightShadowMapShader); }

inline uint32_t LoadModel(const std::string& path, bool gamma = false){ return GetResourceManager().LoadModel(path, gamma); }
inline uint32_t LoadModel(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false){ return GetResourceManager().LoadModel(meshes, model_name, gamma); }
inline void LoadModel(uint32_t id, const std::string& path, bool gamma = false){ GetResourceManager().LoadModel(id, path, gamma); }
inline void LoadModel(uint32_t id, const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma = false){ GetResourceManager().LoadModel(id, meshes, model_name, gamma); }
inline uint32_t LoadSkinnedModel(const std::string& path, const std::string& animationPath, float ticksPerSecond = 0.0f, bool gamma = false){ return GetResourceManager().LoadSkinnedModel(path, animationPath, ticksPerSecond, gamma); }
inline void LoadSkinnedModel(uint32_t id, const std::string& path, const std::string& animationPath, float ticksPerSecond = 0.0f, bool gamma = false){ GetResourceManager().LoadSkinnedModel(id, path, animationPath, ticksPerSecond, gamma); }
inline uint32_t LoadTexture(const std::string& path, bool flip = true){ return GetResourceManager().LoadTexture(path, flip); }
inline uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true){ return GetResourceManager().LoadTexture(path, type, flip); }
inline uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path){ return GetResourceManager().LoadShader(vertex_path, fragment_path); }
inline uint32_t LoadDirectionalLight(const glm::vec3& direction, const glm::vec3& color){ return GetResourceManager().LoadDirectionalLight(direction, color); }
inline uint32_t LoadPointLight(const glm::vec3& position, const glm::vec3& color){ return GetResourceManager().LoadPointLight(position, color); }
inline uint32_t LoadSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float cutOff, float outerCutOff){ return GetResourceManager().LoadSpotLight(position, direction, color, cutOff, outerCutOff); }

inline void UnloadModel(uint32_t id){ GetResourceManager().UnloadModel(id); }
inline void UnloadSkinnedModel(uint32_t id){ GetResourceManager().UnloadSkinnedModel(id); }
inline void UnloadTexture(uint32_t id){ GetResourceManager().UnloadTexture(id); }
inline void UnloadShader(uint32_t id){ GetResourceManager().UnloadShader(id); }
inline void UnloadDirectionalLight(uint32_t id){ GetResourceManager().UnloadDirectionalLight(id); }
inline void UnloadPointLight(uint32_t id){ GetResourceManager().UnloadPointLight(id); }
inline void UnloadSpotLight(uint32_t id){ GetResourceManager().UnloadSpotLight(id); }

inline void UnloadModelsWithoutTransforms(){ GetResourceManager().UnloadModelsWithoutTransforms(); }

inline std::unordered_map<uint32_t, Model>& GetModels() { return GetResourceManager().GetModels(); }
inline std::unordered_map<uint32_t, SkinnedModel>& GetSkinnedModels() { return GetResourceManager().GetSkinnedModels(); }
inline std::unordered_map<uint32_t, Texture>& GetTextures() { return GetResourceManager().GetTextures(); }
inline std::unordered_map<uint32_t, Shader>& GetShaders() { return GetResourceManager().GetShaders(); }
inline std::unordered_map<uint32_t, DirectionalLight>& GetDirectionalLights() { return GetResourceManager().GetDirectionalLights(); }
inline std::unordered_map<uint32_t, PointLight>& GetPointLights() { return GetResourceManager().GetPointLights(); }
inline std::unordered_map<uint32_t, SpotLight>& GetSpotLights() { return GetResourceManager().GetSpotLights(); }
inline unsigned int GetShadowMapArray() { return GetResourceManager().GetShadowMapArray(); }
inline unsigned int GetCubeShadowMapArray() { return GetResourceManager().GetCubeShadowMapArray(); }

inline void HotReloadShaders(){ GetResourceManager().HotReloadShaders(); }
extern void ClearModels();
inline void DrawModels(Shader& shader, glm::mat4 view){ GetResourceManager().DrawModels(shader, view); }
inline void DrawModelsShadows(Shader& shader, glm::mat4 light_space_matrix){ GetResourceManager().DrawModelsShadows(shader, light_space_matrix); }
inline void DrawShadowMaps(){ GetResourceManager().DrawShadowMaps(); }
inline void SetShadowMaps(){ GetResourceManager().SetShadowMaps(); }

inline int GetShadowMapArrayIndex() { return GetResourceManager().GetShadowMapArrayIndex(); }
inline int GetCubeShadowMapArrayIndex() { return GetResourceManager().GetCubeShadowMapArrayIndex(); }
inline void FreeShadowMapArrayIndex(int index) { GetResourceManager().FreeShadowMapArrayIndex(index); }
inline void FreeCubeShadowMapArrayIndex(int index) { GetResourceManager().FreeCubeShadowMapArrayIndex(index); }

inline void UpdateAnimations(float deltaTime) { GetResourceManager().UpdateAnimations(deltaTime); }