#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Material{
    uint32_t albedo;
    uint32_t normal;
    uint32_t metallic;
    uint32_t roughness;
    uint32_t ao;

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