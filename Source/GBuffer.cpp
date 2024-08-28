#include <GBuffer.hpp>
#include <Log.hpp>

#include <glad/glad.h>

GBuffer::GBuffer(unsigned int width, unsigned int height)
{
    Init(width, height);
}

void GBuffer::Init(unsigned int width, unsigned int height)
{
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_PositionTexture);
    glBindTexture(GL_TEXTURE_2D, m_PositionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PositionTexture, 0);

    glGenTextures(1, &m_NormalTexture);
    glBindTexture(GL_TEXTURE_2D, m_NormalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalTexture, 0);

    glGenTextures(1, &m_AlbedoTexture);
    glBindTexture(GL_TEXTURE_2D, m_AlbedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_AlbedoTexture, 0);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    CheckStatus();
}

void GBuffer::Deinit()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_PositionTexture);
    glDeleteTextures(1, &m_NormalTexture);
    glDeleteTextures(1, &m_AlbedoTexture);
    glDeleteRenderbuffers(1, &m_RBO);
}

void GBuffer::CheckStatus()
{
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LogError("Framebuffer is not complete");
    }
}

void GBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void GBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}