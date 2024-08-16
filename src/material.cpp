#include <material.hpp>
#include <resource_manager.hpp>
#include <cstdio>
#include <cstring>
#include <stb_image.h>

void Material::Load(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "r");
    if(!file){
        printf("Failed to open file %s\n", path.c_str());
        return;
    }

    std::string dir = path.substr(0, path.find_last_of('/'));

    char texture_type[32];
    char texture_path[256];

    while(fscanf(file, "%s %s", texture_type, texture_path) != EOF){
        if(strcmp(texture_type, "Albedo") == 0){
            albedo = LoadTexture(dir + "/" + texture_path, std::string("albedoMap"));
        }else if(strcmp(texture_type, "Normal") == 0){
            normal = LoadTexture(dir + "/" + texture_path, std::string("normalMap"));
        }else if(strcmp(texture_type, "Metallic") == 0){
            metallic = LoadTexture(dir + "/" + texture_path, std::string("metallicMap"));
        }else if(strcmp(texture_type, "Roughness") == 0){
            roughness = LoadTexture(dir + "/" + texture_path, std::string("roughnessMap"));
        }else if(strcmp(texture_type, "AO") == 0){ //currently not used
            //ao = LoadTexture(dir + "/" + texture_path, std::string("aoMap"));
        }
    }

    fclose(file);
}

void Material::Load(const std::string& albedo_path, const std::string& normal_path, 
                    const std::string& metallic_path, const std::string& roughness_path, 
                    const std::string& ao_path)
{
    albedo = LoadTexture(albedo_path, "albedoMap");
    normal = LoadTexture(normal_path, "normalMap");
    metallic = LoadTexture(metallic_path, "metallicMap");
    roughness = LoadTexture(roughness_path, "roughnessMap");
    ao = LoadTexture(ao_path, "aoMap");
}

std::vector<uint32_t> Material::GetTextures() const
{
    std::vector<uint32_t> textures;
    
    if(albedo != std::numeric_limits<uint32_t>::max()){
        textures.push_back(albedo);
    }

    if(normal != std::numeric_limits<uint32_t>::max()){
        textures.push_back(normal);
    }

    if(metallic != std::numeric_limits<uint32_t>::max()){
        textures.push_back(metallic);
    }

    if(roughness != std::numeric_limits<uint32_t>::max()){
        textures.push_back(roughness);
    }

    if(ao != std::numeric_limits<uint32_t>::max()){
        textures.push_back(ao);
    }

    return textures;
}