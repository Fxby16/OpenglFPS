#pragma once

#include <unordered_map>

#include <model.hpp>
#include <texture.hpp>
#include <random.hpp>
#include <shader.hpp>

class ResourceManager{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void Deinit();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    inline Model& GetModel(uint32_t id) { return m_Models[id]; }
    inline Texture& GetTexture(uint32_t id) { return m_Textures[id]; }
    inline Shader& GetShader(uint32_t id) { return m_Shaders[id]; }

    uint32_t LoadModel(const std::string& path, bool is_compressed = true, bool pbr = true, bool gamma = false);
    uint32_t LoadModel(const std::vector<Mesh>& meshes, bool is_compressed = true, bool pbr = true, bool gamma = false);
    uint32_t LoadTexture(const std::string& path, bool flip = true);
    uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);
    uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path);

    void UnloadModel(uint32_t id);
    void UnloadTexture(uint32_t id);
    void UnloadShader(uint32_t id);

    inline const std::unordered_map<uint32_t, Model>& GetModels() const { return m_Models; }
    inline const std::unordered_map<uint32_t, Texture>& GetTextures() const { return m_Textures; }
    inline const std::unordered_map<uint32_t, Shader>& GetShaders() const { return m_Shaders; }
    
private:

    std::unordered_map<uint32_t, Model> m_Models;
    std::unordered_map<uint32_t, Texture> m_Textures;
    std::unordered_map<uint32_t, Shader> m_Shaders;
};

extern void DeinitResourceManager();
extern ResourceManager& GetResourceManager();

inline Model& GetModel(uint32_t id) { return GetResourceManager().GetModel(id); }
inline Texture& GetTexture(uint32_t id) { return GetResourceManager().GetTexture(id); }
inline Shader& GetShader(uint32_t id) { return GetResourceManager().GetShader(id); }

extern uint32_t g_GBufferShader, g_DeferredShader;
inline Shader& GetGBufferShader() { return GetShader(g_GBufferShader); }
inline Shader& GetDeferredShader() { return GetShader(g_DeferredShader); }

extern uint32_t LoadModel(const std::string& path, bool is_compressed = true, bool pbr = true, bool gamma = false);
extern uint32_t LoadModel(const std::vector<Mesh>& meshes, bool is_compressed = true, bool pbr = true, bool gamma = false);
extern uint32_t LoadTexture(const std::string& path, bool flip = true);
extern uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);
extern uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path);

extern void UnloadModel(uint32_t id);
extern void UnloadTexture(uint32_t id);
extern void UnloadShader(uint32_t id);

inline const std::unordered_map<uint32_t, Model>& GetModels() { return GetResourceManager().GetModels(); }
inline const std::unordered_map<uint32_t, Texture>& GetTextures() { return GetResourceManager().GetTextures(); }
inline const std::unordered_map<uint32_t, Shader>& GetShaders() { return GetResourceManager().GetShaders(); }