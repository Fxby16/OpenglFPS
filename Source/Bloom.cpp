#include <Bloom.hpp>
#include <Renderer.hpp>
#include <Globals.hpp>
#include <ComputeShader.hpp>

#include <limits>

#include <glad/glad.h>

static unsigned int g_BloomTexture = std::numeric_limits<unsigned int>::max();
static unsigned int g_BloomTexView = std::numeric_limits<unsigned int>::max();
static float g_BloomThreshold = 100000.0f;
static float g_BloomKnee = 0.5f;
static ComputeShader g_BloomFilterShader;
static ComputeShader g_BloomDownsampleShader;
static ComputeShader g_BloomUpsampleShader;

void InitBloom()
{
    glGenTextures(1, &g_BloomTexture);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, g_BloomTexture);

    glTexStorage2D(GL_TEXTURE_2D, NUM_MIPS, GL_RGBA32F, g_ScreenWidth, g_ScreenHeight);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &g_BloomTexView);
    glTextureView(g_BloomTexView, GL_TEXTURE_2D, g_BloomTexture, GL_RGBA32F, 0, 1, 0, 1);

    g_BloomFilterShader.Load("Resources/Shaders/BloomFilter.comp");
    g_BloomDownsampleShader.Load("Resources/Shaders/BloomDownsampling.comp");
    g_BloomUpsampleShader.Load("Resources/Shaders/BloomUpsampling.comp");
}

void DeinitBloom()
{
    glDeleteTextures(1, &g_BloomTexture);
    glDeleteTextures(1, &g_BloomTexView);

    g_BloomFilterShader.Unload();
    g_BloomDownsampleShader.Unload();
    g_BloomUpsampleShader.Unload();
}

void BloomPass()
{
    Framebuffer& fbo = GetDeferredPassFramebuffer();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo.GetFBO());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_BloomTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_ScreenWidth, g_ScreenHeight);

    g_BloomFilterShader.Bind();
    g_BloomFilterShader.SetUniform1f("gBloomThreshold", g_BloomThreshold);
    g_BloomFilterShader.SetUniform1f("gBloomSoftKnee", g_BloomKnee);

    glBindImageTexture(0, g_BloomTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    
    int groupCountX = glm::ceil(g_ScreenWidth / 16.0f);
    int groupCountY = glm::ceil(g_ScreenHeight / 16.0f);
    glDispatchCompute(groupCountX, groupCountY, 1);
    
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    g_BloomDownsampleShader.Bind();
    for(int i = 0; i < NUM_MIPS - 1; i++){
        glBindImageTexture(0, g_BloomTexture, i, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, g_BloomTexture, i + 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        g_BloomDownsampleShader.SetUniform2i("srcResolution", g_ScreenWidth / (1 << i), g_ScreenHeight / (1 << i));

        int width = g_ScreenWidth / (1 << (i + 1));
        int height = g_ScreenHeight / (1 << (i + 1));

        groupCountX = glm::ceil(width / 16.0f);
        groupCountY = glm::ceil(height / 16.0f);

        glDispatchCompute(groupCountX, groupCountY, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    g_BloomUpsampleShader.Bind();
    for(int i = NUM_MIPS - 1; i > 0; i--){
        glBindImageTexture(0, g_BloomTexture, i, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, g_BloomTexture, i - 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        g_BloomUpsampleShader.SetUniform2i("srcResolution", g_ScreenWidth / (1 << i), g_ScreenHeight / (1 << i));

        int width = g_ScreenWidth / (1 << (i - 1));
        int height = g_ScreenHeight / (1 << (i - 1));

        int groupCountX = glm::ceil(width / 16.0f);
        int groupCountY = glm::ceil(height / 16.0f);

        glDispatchCompute(groupCountX, groupCountY, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int GetBloomTexture()
{
    return g_BloomTexView;
}

void SetBloomThreshold(float threshold)
{
    g_BloomThreshold = threshold;
}

float GetBloomThreshold()
{
    return g_BloomThreshold;
}

void SetBloomKnee(float knee)
{
    g_BloomKnee = knee;
}

float GetBloomKnee()
{
    return g_BloomKnee;
}