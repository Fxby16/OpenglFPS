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

void ResourceManager::LoadModel(uint32_t id, const std::string& path, bool gamma)
{
    m_Models[id].Load(path, gamma);
}

void ResourceManager::LoadModel(uint32_t id, const std::vector<Mesh>& meshes, const std::string& model_name, bool gamma)
{
    m_Models[id].Load(meshes, model_name, gamma);
}

uint32_t ResourceManager::LoadSkinnedModel(const std::string& path, const std::string& animationPath, float ticksPerSecond, bool gamma)
{
    uint32_t id = RandUint32();
    m_SkinnedModels[id].model.Load(path, gamma);
    m_SkinnedModels[id].animator = Animator(animationPath, m_SkinnedModels[id].model, ticksPerSecond);
    return id;
}

void ResourceManager::LoadSkinnedModel(uint32_t id, const std::string& path, const std::string& animationPath, float ticksPerSecond, bool gamma)
{
    m_SkinnedModels[id].model.Load(path, gamma);
    m_SkinnedModels[id].animator = Animator(animationPath, m_SkinnedModels[id].model, ticksPerSecond);
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

uint32_t ResourceManager::LoadDirectionalLight(const glm::vec3& direction, const glm::vec3& color)
{
    uint32_t id = RandUint32();
    m_DirectionalLights[id] = DirectionalLight(direction, color);
    return id;
}

uint32_t ResourceManager::LoadPointLight(const glm::vec3& position, const glm::vec3& color)
{
    uint32_t id = RandUint32();
    m_PointLights[id] = PointLight(position, color);
    return id;
}

uint32_t ResourceManager::LoadSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float cutOff, float outerCutOff)
{
    uint32_t id = RandUint32();
    m_SpotLights[id] = SpotLight(position, direction, color, cutOff, outerCutOff);
    return id;
}

void ResourceManager::UnloadModel(uint32_t id)
{
    m_Models[id].Unload();
    m_Models.erase(id);
}

void ResourceManager::UnloadSkinnedModel(uint32_t id)
{
    m_SkinnedModels[id].model.Unload();
    m_SkinnedModels.erase(id);
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

void ResourceManager::UnloadDirectionalLight(uint32_t id)
{
    m_DirectionalLights[id].DeinitShadowMap();
    m_DirectionalLights.erase(id);
}

void ResourceManager::UnloadPointLight(uint32_t id)
{
    m_PointLights[id].DeinitShadowMap();
    m_PointLights.erase(id);
}

void ResourceManager::UnloadSpotLight(uint32_t id)
{
    m_SpotLights[id].DeinitShadowMap();
    m_SpotLights.erase(id);
}

void ResourceManager::Init()
{
    g_Cube = 0;
    LoadModel(g_Cube, {CUBE_MESH}, "CUBE", false);
    g_Sphere = 1;
    LoadModel(g_Sphere, {SPHERE_MESH}, "SPHERE", false);

    Material mat;
    mat.Load("Resources/Materials/lined_cement/lined_cement.mat");

    auto& cube_meshes = GetModel(g_Cube)->GetMeshes();
    for(auto& mesh : cube_meshes){
        mesh.SetMaterial(mat);
    }

    auto& sphere_meshes = GetModel(g_Sphere)->GetMeshes();
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

    for(auto& [id, skinned_model] : m_SkinnedModels){
        skinned_model.model.Unload();
    }

    for(auto& [id, texture] : m_Textures){
        texture.Free();
    }

    for(auto& [id, shader] : m_Shaders){
        shader.Unload();
    }

    for(auto& [id, directional_light] : m_DirectionalLights){
        directional_light.DeinitShadowMap();
    }

    for(auto& [id, point_light] : m_PointLights){
        point_light.DeinitShadowMap();
    }

    for(auto& [id, spot_light] : m_SpotLights){
        spot_light.DeinitShadowMap();
    }

    m_Models.clear();
    m_SkinnedModels.clear();
    m_Textures.clear();
    m_Shaders.clear();
    m_DirectionalLights.clear();
    m_PointLights.clear();
    m_SpotLights.clear();

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

void ResourceManager::DrawModels(Shader& shader, glm::mat4 view)
{
    for(auto& [id, model] : GetModels()){
        auto& transforms = model.GetTransforms();

        for(uint32_t i = 0; i < transforms.size(); i++){
            model.Draw(shader, view, transforms[i]);
        }
    }

    for(auto& [id, skinned_model] : GetSkinnedModels()){
        auto& transforms = skinned_model.model.GetTransforms();

        for(uint32_t i = 0; i < transforms.size(); i++){
            skinned_model.animator.UploadFinalBoneMatrices(shader);
            skinned_model.model.Draw(shader, view, transforms[i]);
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

    for(auto& [id, skinned_model] : GetSkinnedModels()){
        auto& transforms = skinned_model.model.GetTransforms();

        for(uint32_t i = 0; i < transforms.size(); i++){
            skinned_model.animator.UploadFinalBoneMatrices(shader);
            skinned_model.model.DrawShadows(shader, light_space_matrix, transforms[i]);
        }
    }
}

void ResourceManager::DrawShadowMaps()
{
    for(auto& [id, directional_light] : GetDirectionalLights()){
        DrawShadowMap(directional_light);
    }

    for(auto& [id, point_light] : GetPointLights()){
        DrawShadowMap(point_light);
    }

    for(auto& [id, spot_light] : GetSpotLights()){
        DrawShadowMap(spot_light);
    }
}

void ResourceManager::SetShadowMaps()
{
    GetDeferredShader().Bind();
    ResetLightsCounters();

    for(auto& [id, directional_light] : GetDirectionalLights()){
        SetDirectionalLight(directional_light);
    }

    for(auto& [id, point_light] : GetPointLights()){
        SetPointLight(point_light);
    }

    for(auto& [id, spot_light] : GetSpotLights()){
        SetSpotLight(spot_light);
    }
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

    for(auto it = m_SkinnedModels.begin(); it != m_SkinnedModels.end();){
        if(it->second.model.GetTransforms().empty()){
            it->second.model.Unload();
            it = m_SkinnedModels.erase(it);
        }else{
            ++it;
        }
    }
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

    for(auto it = g_ResourceManager.GetSkinnedModels().begin(); it != g_ResourceManager.GetSkinnedModels().end();){
        it->second.model.Unload();
        it = g_ResourceManager.GetSkinnedModels().erase(it);
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

void ResourceManager::UpdateAnimations(float deltaTime)
{
    for(auto& [id, skinned_model] : m_SkinnedModels){
        skinned_model.animator.Update(deltaTime);
    }
}