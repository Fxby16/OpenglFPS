#include <lights.hpp>
#include <resource_manager.hpp>

static unsigned int g_PointLightsCount = 0;
static unsigned int g_SpotLightsCount = 0;
static unsigned int g_DirectionalLightsCount = 0;
constexpr unsigned int MAX_LIGHTS = 16;

void ResetCounters()
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
        GetDeferredShader().SetUniform1i("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].shadowMap", 5); // 5 is the texture unit, needs to be changed
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
        g_SpotLightsCount++;
        GetDeferredShader().SetUniform1i("numSpotLights", g_SpotLightsCount);
    }
}