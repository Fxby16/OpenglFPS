#include <Framebuffer.hpp>
#include <Log.hpp>
#include <Window.hpp>

#include <glad/glad.h>
#include <cstdint>

Framebuffer::Framebuffer(int width, int height)
{
    Init(width, height);
    m_ResizeCallbackID = AddWindowResizeCallback(std::bind(&Framebuffer::Resize, this, std::placeholders::_1, std::placeholders::_2));
}

Framebuffer::~Framebuffer()
{
    RemoveWindowResizeCallback(m_ResizeCallbackID);
}

void Framebuffer::Init(int width, int height)
{
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_ColorBufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_ColorBufferTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBufferTexture, 0);

    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    CheckStatus();
}

void Framebuffer::Deinit()
{
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_ColorBufferTexture);
    glDeleteRenderbuffers(1, &m_RBO);
}

void Framebuffer::CheckStatus()
{
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LogError("Framebuffer is not complete");
    }
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int width, int height)
{
    Deinit();
    Init(width, height);
}