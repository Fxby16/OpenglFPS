#include <ResourceManager.hpp>
#include <PredefinedMeshes.hpp>
#include <Material.hpp>
#include <Globals.hpp>
#include <ShadowMap.hpp>

#include <stb_image.h>
#include <glad/glad.h>

static ResourceManager g_ResourceManager;
uint32_t g_GBufferShader, g_DeferredShader, g_ShadowMapShader, g_PointLightShadowMapShader;

uint32_t g_Cube, g_Sphere;

uint32_t ResourceManager::LoadModel(const std::string& path, bool gamma)
{
    uint32_t id = RandUint32();
    m_Models[id].Load(path, gamma);
    return id;
}

uint32_t ResourceManager::LoadModel(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma)
{
    uint32_t id = RandUint32();
    m_Models[id].Load(meshes, model_name, gamma);
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

void ResourceManager::Init()
{
    g_Cube = LoadModel({CUBE_MESH}, "CUBE", false);
    g_Sphere = LoadModel({SPHERE_MESH}, "SPHERE", false);

    Material mat;
    mat.Load("Resources/Materials/lined_cement/lined_cement.mat");

    auto& cube_meshes = GetModel(g_Cube).GetMeshes();
    for(auto& mesh : cube_meshes){
        mesh.SetMaterial(mat);
    }

    auto& sphere_meshes = GetModel(g_Sphere).GetMeshes();
    for(auto& mesh : sphere_meshes){
        mesh.SetMaterial(mat);
    }

    m_ShadowMapArray = InitShadowMapArray(MAX_LIGHTS * 2); // directional + spot lights
    m_CubeShadowMapArray = InitCubeShadowMapArray(MAX_LIGHTS); // point lights

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMapArray);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_CubeShadowMapArray);

    for(int i = 0; i < 20; i++){
        m_ShadowMapArrayIndices.insert(i);
    }

    for(int i = 0; i < 10; i++){
        m_CubeShadowMapArrayIndices.insert(i);
    }
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

    glDeleteTextures(1, &m_ShadowMapArray);
    glDeleteTextures(1, &m_CubeShadowMapArray);
}

void InitResourceManager()
{
    g_ResourceManager.Init();
}

void DeinitResourceManager()
{
    g_ResourceManager.Deinit();
}

ResourceManager& GetResourceManager()
{
    return g_ResourceManager;
}

uint32_t LoadModel(const std::string& path, bool gamma)
{
    return g_ResourceManager.LoadModel(path, gamma);
}

uint32_t LoadModel(const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma)
{
    return g_ResourceManager.LoadModel(meshes, model_name, gamma);
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

void ResourceManager::HotReloadShaders()
{
    for(auto& [id, shader] : GetShaders()){
        shader.Reload();
    }

    Shader& gbuffer_s = GetGBufferShader();
    gbuffer_s.Bind();
    gbuffer_s.SetUniformMat4fv("projection", GetCamera().GetProjectionMatrix(), 1);

    Shader& deferred_s = GetDeferredShader();
    deferred_s.Bind();
    deferred_s.SetUniform1i("Positions", 0);
    deferred_s.SetUniform1i("Normals", 1);
    deferred_s.SetUniform1i("Albedo", 2);
}

void HotReloadShaders()
{
    g_ResourceManager.HotReloadShaders();
}

void ResourceManager::DrawModels(Shader& shader, glm::mat4 view)
{
    for(auto& [id, model] : GetModels()){
        auto& transforms = model.GetTransforms();

        for(uint32_t i = 0; i < transforms.size(); i++){
            model.Draw(shader, view, transforms[i]);
        }
    }
}

void ResourceManager::DrawModelsShadows(Shader& shader, glm::mat4 light_space_matrix)
{
    for(auto& [id, model] : GetModels()){
        auto& transforms = model.GetTransforms();

        for(uint32_t i = 0; i < transforms.size(); i++){
            model.DrawShadows(shader, light_space_matrix, transforms[i]);
        }
    }
}

void DrawModels(Shader& shader, glm::mat4 view)
{
    g_ResourceManager.DrawModels(shader, view);
}

void DrawModelsShadows(Shader& shader, glm::mat4 light_space_matrix)
{
    g_ResourceManager.DrawModelsShadows(shader, light_space_matrix);
}

void ResourceManager::UnloadModelsWithoutTransforms()
{
    for(auto it = m_Models.begin(); it != m_Models.end();){
        if(it->second.GetTransforms().empty() && it->first != g_Cube && it->first != g_Sphere){
            it->second.Unload();
            it = m_Models.erase(it);
        }else{
            ++it;
        }
    }
}

void UnloadModelsWithoutTransforms()
{
    g_ResourceManager.UnloadModelsWithoutTransforms();
}

void ClearModels()
{
    for(auto it = g_ResourceManager.GetModels().begin(); it != g_ResourceManager.GetModels().end();){
        if(it->first != g_Cube && it->first != g_Sphere){
            it->second.Unload();
            it = g_ResourceManager.GetModels().erase(it);
        }else{
            it->second.ClearTransforms();
            ++it;
        }
    }
}

void ResourceManager::FreeShadowMapArrayIndex(int index)
{
    m_ShadowMapArrayIndices.insert(index);
}

void ResourceManager::FreeCubeShadowMapArrayIndex(int index)
{
    m_CubeShadowMapArrayIndices.insert(index);
}

int ResourceManager::GetShadowMapArrayIndex()
{
    if(m_ShadowMapArrayIndices.empty()){
        return -1;
    }

    int index = *m_ShadowMapArrayIndices.begin();
    m_ShadowMapArrayIndices.erase(index);
    return index;
}

int ResourceManager::GetCubeShadowMapArrayIndex()
{
    if(m_CubeShadowMapArrayIndices.empty()){
        return -1;
    }

    int index = *m_CubeShadowMapArrayIndices.begin();
    m_CubeShadowMapArrayIndices.erase(index);
    return index;
}