#include <resource_manager.hpp>

#include <stb_image.h>

static ResourceManager g_ResourceManager;
uint32_t g_GBufferShader, g_DeferredShader;

uint32_t ResourceManager::LoadModel(const std::string& path, bool is_compressed, bool pbr, bool gamma)
{
    uint32_t id = RandUint32();
    m_Models[id].Load(path, is_compressed, pbr, gamma);
    return id;
}

uint32_t ResourceManager::LoadModel(const std::vector<Mesh>& meshes, bool is_compressed, bool pbr, bool gamma)
{
    uint32_t id = RandUint32();
    m_Models[id].Load(meshes, is_compressed, pbr, gamma);
    return id;
}

uint32_t ResourceManager::LoadTexture(const std::string& path, bool flip)
{
    uint32_t id = RandUint32();
    m_Textures[id].Init(path, flip);
    return id;
}

uint32_t ResourceManager::LoadTexture(const std::string& path, const std::string& type, bool flip)
{
    uint32_t id = RandUint32();
    m_Textures[id].Init(path, type, flip);
    return id;
}

uint32_t ResourceManager::LoadShader(const std::string& vertex_path, const std::string& fragment_path)
{
    uint32_t id = RandUint32();
    m_Shaders[id].Load(vertex_path.c_str(), fragment_path.c_str());
    return id;
}

void ResourceManager::UnloadModel(uint32_t id)
{
    m_Models[id].Unload();
    m_Models.erase(id);
}

void ResourceManager::UnloadTexture(uint32_t id)
{
    m_Textures[id].Free();
    m_Textures.erase(id);
}

void ResourceManager::UnloadShader(uint32_t id)
{
    m_Shaders[id].Unload();
    m_Shaders.erase(id);
}

void ResourceManager::Deinit()
{
    for(auto& [id, model] : m_Models){
        model.Unload();
    }

    for(auto& [id, texture] : m_Textures){
        texture.Free();
    }

    for(auto& [id, shader] : m_Shaders){
        shader.Unload();
    }

    m_Models.clear();
    m_Textures.clear();
    m_Shaders.clear();
}

void DeinitResourceManager()
{
    g_ResourceManager.Deinit();
}

ResourceManager& GetResourceManager()
{
    return g_ResourceManager;
}

uint32_t LoadModel(const std::string& path, bool is_compressed, bool pbr, bool gamma)
{
    return g_ResourceManager.LoadModel(path, is_compressed, pbr, gamma);
}

uint32_t LoadModel(const std::vector<Mesh>& meshes, bool is_compressed, bool pbr, bool gamma)
{
    return g_ResourceManager.LoadModel(meshes, is_compressed, pbr, gamma);
}

uint32_t LoadTexture(const std::string& path, bool flip)
{
    return g_ResourceManager.LoadTexture(path, flip);
}

uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip)
{
    return g_ResourceManager.LoadTexture(path, type, flip);
}

void UnloadModel(uint32_t id)
{
    g_ResourceManager.UnloadModel(id);
}

void UnloadTexture(uint32_t id)
{
    g_ResourceManager.UnloadTexture(id);
}

uint32_t LoadShader(const std::string& vertex_path, const std::string& fragment_path)
{
    return g_ResourceManager.LoadShader(vertex_path, fragment_path);
}

void UnloadShader(uint32_t id)
{
    g_ResourceManager.UnloadShader(id);
}