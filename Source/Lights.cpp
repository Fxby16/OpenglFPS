#include <Lights.hpp>
#include <ResourceManager.hpp>
#include <Globals.hpp>

static unsigned int g_PointLightsCount = 0;
static unsigned int g_SpotLightsCount = 0;
static unsigned int g_DirectionalLightsCount = 0;

void ResetLightsCounters()
{
    g_PointLightsCount = 0;
    g_SpotLightsCount = 0;
    g_DirectionalLightsCount = 0;

    GetDeferredShader().Bind();
    GetDeferredShader().SetUniform1i("numPointLights", 0);
    GetDeferredShader().SetUniform1i("numSpotLights", 0);
    GetDeferredShader().SetUniform1i("numDirectionalLights", 0);
}

void SetPointLight(const PointLight& pl)
{
    if(g_PointLightsCount < MAX_LIGHTS){
        GetDeferredShader().Bind();
        GetDeferredShader().SetUniform3fv("pointLights[" + std::to_string(g_PointLightsCount) + "].position", pl.pos);
        GetDeferredShader().SetUniform3fv("pointLights[" + std::to_string(g_PointLightsCount) + "].color", pl.color);
        for(unsigned int i = 0; i < MAX_LIGHTS; i++){
            GetDeferredShader().SetUniform1i("pointLights[" + std::to_string(i) + "].shadowMapIndex", pl.shadowMap.GetShadowMapIndex());
        }
        g_PointLightsCount++;
        GetDeferredShader().SetUniform1i("numPointLights", g_PointLightsCount);
    }
}

void SetDirectionalLight(const DirectionalLight& dl)
{
    if(g_DirectionalLightsCount < MAX_LIGHTS){
        GetDeferredShader().Bind();
        GetDeferredShader().SetUniform3fv("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].direction", dl.dir);
        GetDeferredShader().SetUniform3fv("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].color", dl.color);
        GetDeferredShader().SetUniform1i("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].shadowMapIndex", dl.shadowMap.GetShadowMapIndex());
        GetDeferredShader().SetUniformMat4fv("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].lightSpaceMatrix", dl.lightSpaceMatrix);
        g_DirectionalLightsCount++;
        GetDeferredShader().SetUniform1i("numDirectionalLights", g_DirectionalLightsCount);
    }
}

void SetSpotLight(const SpotLight& dl)
{
    if(g_SpotLightsCount < MAX_LIGHTS){
        GetDeferredShader().Bind();
        GetDeferredShader().SetUniform3fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].position", dl.pos);
        GetDeferredShader().SetUniform3fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].direction", dl.dir);
        GetDeferredShader().SetUniform3fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].color", dl.color);
        GetDeferredShader().SetUniform1f("spotLights[" + std::to_string(g_SpotLightsCount) + "].cutOff", dl.cutOff);
        GetDeferredShader().SetUniform1f("spotLights[" + std::to_string(g_SpotLightsCount) + "].outerCutOff", dl.outerCutOff);
        GetDeferredShader().SetUniform1i("spotLights[" + std::to_string(g_SpotLightsCount) + "].shadowMapIndex", dl.shadowMap.GetShadowMapIndex());
        GetDeferredShader().SetUniformMat4fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].lightSpaceMatrix", dl.lightSpaceMatrix);
        g_SpotLightsCount++;
        GetDeferredShader().SetUniform1i("numSpotLights", g_SpotLightsCount);
    }
}

void DrawShadowMap(const DirectionalLight& light)
{
    light.shadowMap.Bind(); 
    DrawModelsShadows(GetShadowMapShader(), light.lightSpaceMatrix);
    light.shadowMap.Unbind();
}

void DrawShadowMap(const SpotLight& light)
{
    light.shadowMap.Bind();
    DrawModelsShadows(GetShadowMapShader(), light.lightSpaceMatrix);
    light.shadowMap.Unbind();
}

void DrawShadowMap(const PointLight& light)
{
    GetPointLightShadowMapShader().Bind();
    GetPointLightShadowMapShader().SetUniform3fv("lightPos", light.pos);

    for(int i = 0; i < 6; i++){
        light.shadowMap.Bind(i);
        DrawModelsShadows(GetPointLightShadowMapShader(), light.lightSpaceMatrix[i]);
    }

    light.shadowMap.Unbind();
}