#pragma once

#include <unordered_map>

#include <model.hpp>
#include <texture.hpp>
#include <random.hpp>

class ResourceManager{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void Deinit();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    Model& GetModel(uint32_t id);
    Texture& GetTexture(uint32_t id);

    uint32_t LoadModel(const std::string& path, bool pbr = true, bool gamma = false);
    uint32_t LoadTexture(const std::string& path, bool flip = true);
    uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);

    void UnloadModel(uint32_t id);
    void UnloadTexture(uint32_t id);
    
private:

    std::unordered_map<uint32_t, Model> m_Models;
    std::unordered_map<uint32_t, Texture> m_Textures;
};

extern void DeinitResourceManager();

extern Model& GetModel(uint32_t id);
extern Texture& GetTexture(uint32_t id);

extern uint32_t LoadModel(const std::string& path, bool pbr = true, bool gamma = false);
extern uint32_t LoadTexture(const std::string& path, bool flip = true);
extern uint32_t LoadTexture(const std::string& path, const std::string& type, bool flip = true);

extern void UnloadModel(uint32_t id);
extern void UnloadTexture(uint32_t id);