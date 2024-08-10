#include <resource_manager.hpp>

#include <stb_image.h>

static ResourceManager g_ResourceManager;

Model& ResourceManager::GetModel(uint32_t id)
{
    return m_Models[id];
}

Texture& ResourceManager::GetTexture(uint32_t id)
{
    return m_Textures[id];
}

uint32_t ResourceManager::LoadModel(const std::string& path, bool pbr, bool gamma)
{
    uint32_t id = RandUint32();
    m_Models[id].Load(path, pbr, gamma);
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

void ResourceManager::Deinit()
{
    for(auto& [id, model] : m_Models){
        model.Unload();
    }

    for(auto& [id, texture] : m_Textures){
        texture.Free();
    }

    m_Models.clear();
    m_Textures.clear();
}

void DeinitResourceManager()
{
    g_ResourceManager.Deinit();
}

Model& GetModel(uint32_t id)
{
    return g_ResourceManager.GetModel(id);
}

Texture& GetTexture(uint32_t id)
{
    return g_ResourceManager.GetTexture(id);
}

uint32_t LoadModel(const std::string& path, bool pbr, bool gamma)
{
    return g_ResourceManager.LoadModel(path, pbr, gamma);
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