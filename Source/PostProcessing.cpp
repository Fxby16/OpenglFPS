#include <PostProcessing.hpp>
#include <ResourceManager.hpp>
#include <Bloom.hpp>
#include <OpenGL.hpp>
#include <Renderer.hpp>

#include <limits> 

static float g_Exposure = 1.0f;
static float g_BloomStrength = 0.28f;
static int g_ToneMapping = REINHARD;
static uint32_t g_DirtTexture = std::numeric_limits<uint32_t>::max();
static uint32_t g_PostProcessingShader = std::numeric_limits<uint32_t>::max();

static bool useBloom = false;

void InitPostProcessing()
{
    g_PostProcessingShader = LoadShader("Resources/Shaders/PostProcessing.vert", "Resources/Shaders/PostProcessing.frag");
    GetShader(g_PostProcessingShader)->Bind();
    GetShader(g_PostProcessingShader)->SetUniform1i("useDirtTexture", 0);
    GetShader(g_PostProcessingShader)->SetUniform1i("bloomBlurTexture", 0);
    GetShader(g_PostProcessingShader)->SetUniform1i("screenTexture", 1);
    GetShader(g_PostProcessingShader)->SetUniform1i("dirtTexture", 2);
    GetShader(g_PostProcessingShader)->SetUniform1f("exposure", g_Exposure);
    GetShader(g_PostProcessingShader)->SetUniform1i("toneMappingType", g_ToneMapping);
    GetShader(g_PostProcessingShader)->SetUniform1f("bloomStrength", g_BloomStrength);
    GetShader(g_PostProcessingShader)->SetUniform1i("useBloom", useBloom);
}

void PostProcessingPass()
{
    BindTexture(GetBloomTexture(), 0);
    BindTexture(GetDeferredPassFramebuffer().GetColorBufferTexture(), 1);   
    if(g_DirtTexture != std::numeric_limits<uint32_t>::max()){
        BindTexture(GetTexture(g_DirtTexture)->GetID(), 2);
    }

    GetShader(g_PostProcessingShader)->Bind();
    DrawFullscreenQuad();
}

void SetExposure(float exposure)
{
    g_Exposure = exposure;
}

float GetExposure()
{
    return g_Exposure;
}

void SetBloomStrength(float intensity)
{
    g_BloomStrength = intensity;
}

float GetBloomStrength()
{
    return g_BloomStrength;
}

void UseBloom(bool use)
{
    useBloom = use;
    GetShader(g_PostProcessingShader)->Bind();
    GetShader(g_PostProcessingShader)->SetUniform1i("useBloom", useBloom);
}

bool GetUseBloom()
{
    return useBloom;
}

void SetToneMapping(int mapping)
{
    g_ToneMapping = mapping;
}

int GetToneMapping()
{
    return g_ToneMapping;
}

void SetDirtTexture(const std::string& path)
{
    if(g_DirtTexture != std::numeric_limits<uint32_t>::max()){
        UnloadTexture(g_DirtTexture);
    }

    g_DirtTexture = LoadTexture(path);
    GetShader(g_PostProcessingShader)->Bind();
    GetShader(g_PostProcessingShader)->SetUniform1i("useDirtTexture", 1);
}