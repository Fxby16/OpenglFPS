#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <limits>

struct Material{
    uint32_t albedo;
    uint32_t normal;
    uint32_t metallic;
    uint32_t roughness;
    uint32_t ao;

    Material()
    {
        albedo = std::numeric_limits<uint32_t>::max();
        normal = std::numeric_limits<uint32_t>::max();
        metallic = std::numeric_limits<uint32_t>::max();
        roughness = std::numeric_limits<uint32_t>::max();
        ao = std::numeric_limits<uint32_t>::max();
    }

    /**
     * @brief Load the material textures. You must provide the path to a file that contains the paths to the textures.
     * TextureType  Path
     */
    void Load(const std::string& path);
    void Load(const std::string& albedo_path, const std::string& normal_path, 
              const std::string& metallic_path, const std::string& roughness_path, 
              const std::string& ao_path);

    std::vector<uint32_t> GetTextures() const;
};