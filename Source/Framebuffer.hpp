#pragma once

#include <cstdint>
#include <limits>

class Framebuffer{
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);
    ~Framebuffer() = default;

    void Init(int width, int height);
    void Deinit();

    void Bind() const;
    void Unbind() const;

    void Resize(int width, int height);

    inline unsigned int GetColorBufferTexture() const { return m_ColorBufferTexture; }
    inline unsigned int GetFBO() const { return m_FBO; }
    inline unsigned int GetRBO() const { return m_RBO; }

private:
    void CheckStatus();

    unsigned int m_FBO;
    unsigned int m_ColorBufferTexture;
    unsigned int m_RBO;

    uint32_t m_ResizeCallbackID = std::numeric_limits<uint32_t>::max();
};