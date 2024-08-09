#include <lights.hpp>

static Shader* g_DeferredShader = nullptr;
static unsigned int g_PointLightsCount = 0;
static unsigned int g_SpotLightsCount = 0;
static unsigned int g_DirectionalLightsCount = 0;
constexpr unsigned int MAX_LIGHTS = 16;

void SetLightShader(Shader* shader)
{
    g_DeferredShader = shader;
}

void ResetCounters()
{
    g_PointLightsCount = 0;
    g_SpotLightsCount = 0;
    g_DirectionalLightsCount = 0;

    g_DeferredShader->Bind();
    g_DeferredShader->SetUniform1i("numPointLights", 0);
    g_DeferredShader->SetUniform1i("numSpotLights", 0);
    g_DeferredShader->SetUniform1i("numDirectionalLights", 0);
}

void SetPointLight(const PointLight& pl)
{
    if(g_PointLightsCount < MAX_LIGHTS){
        g_DeferredShader->Bind();
        g_DeferredShader->SetUniform3fv("pointLights[" + std::to_string(g_PointLightsCount) + "].position", pl.pos);
        g_DeferredShader->SetUniform3fv("pointLights[" + std::to_string(g_PointLightsCount) + "].color", pl.color);
        g_PointLightsCount++;
        g_DeferredShader->SetUniform1i("numPointLights", g_PointLightsCount);
    }
}

void SetDirectionalLight(const DirectionalLight& dl)
{
    if(g_DirectionalLightsCount < MAX_LIGHTS){
        g_DeferredShader->Bind();
        g_DeferredShader->SetUniform3fv("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].direction", dl.dir);
        g_DeferredShader->SetUniform3fv("directionalLights[" + std::to_string(g_DirectionalLightsCount) + "].color", dl.color);
        g_DirectionalLightsCount++;
        g_DeferredShader->SetUniform1i("numDirectionalLights", g_DirectionalLightsCount);
    }
}

void SetSpotLight(const SpotLight& dl)
{
    if(g_SpotLightsCount < MAX_LIGHTS){
        g_DeferredShader->Bind();
        g_DeferredShader->SetUniform3fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].position", dl.pos);
        g_DeferredShader->SetUniform3fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].direction", dl.dir);
        g_DeferredShader->SetUniform3fv("spotLights[" + std::to_string(g_SpotLightsCount) + "].color", dl.color);
        g_DeferredShader->SetUniform1f("spotLights[" + std::to_string(g_SpotLightsCount) + "].cutOff", dl.cutOff);
        g_DeferredShader->SetUniform1f("spotLights[" + std::to_string(g_SpotLightsCount) + "].outerCutOff", dl.outerCutOff);
        g_SpotLightsCount++;
        g_DeferredShader->SetUniform1i("numSpotLights", g_SpotLightsCount);
    }
}