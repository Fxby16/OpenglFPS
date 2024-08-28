#include <ShadowMap.hpp>
#include <Globals.hpp>
#include <ResourceManager.hpp>
#include <Log.hpp>

#include <glad/glad.h>

void ShadowMap::Init()
{
    glGenFramebuffers(1, &m_FBO);

    m_ShadowMapIndex = GetShadowMapArrayIndex();

    if(m_ShadowMapIndex == -1){
        LogWarning("No textures remaining for shadow maps");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GetShadowMapArray(), 0, m_ShadowMapIndex);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LogError("Shadow map framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::Deinit()
{
    glDeleteFramebuffers(1, &m_FBO);
}

void ShadowMap::Bind() const
{
    glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
}

void PointLightShadowMap::Init()
{
    glGenFramebuffers(1, &m_FBO);

    m_ShadowMapIndex = GetCubeShadowMapArrayIndex();

    if(m_ShadowMapIndex == -1){
        LogWarning("No cube maps remaining for point light shadow maps");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GetCubeShadowMapArray(), 0, m_ShadowMapIndex * 6);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LogError("Shadow cube map framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLightShadowMap::Deinit()
{
    glDeleteFramebuffers(1, &m_FBO);
}

void PointLightShadowMap::Bind(unsigned int face) const
{
    glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GetCubeShadowMapArray(), 0, m_ShadowMapIndex * 6 + face);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void PointLightShadowMap::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
}

unsigned int InitShadowMapArray(unsigned int count)
{
    unsigned int shadowMapArray;
    glGenTextures(1, &shadowMapArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    return shadowMapArray;
}

unsigned int InitCubeShadowMapArray(unsigned int count)
{
    unsigned int shadowMapArray;
    glGenTextures(1, &shadowMapArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMapArray);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, count * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    return shadowMapArray;
}