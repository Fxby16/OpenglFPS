#pragma once

class Framebuffer{
public:
    Framebuffer() = default;
    Framebuffer(float width, float height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;

    void Resize(float width, float height);

    inline unsigned int GetColorBufferTexture() const { return m_ColorBufferTexture; }
    inline unsigned int GetFBO() const { return m_FBO; }
    inline unsigned int GetRBO() const { return m_RBO; }

private:
    void Init(float width, float height);
    void Deinit();

    void CheckStatus();

    unsigned int m_FBO;
    unsigned int m_ColorBufferTexture;
    unsigned int m_RBO;
};