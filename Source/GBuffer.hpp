#pragma once

#include <cstdint>
#include <limits>

class GBuffer{
public:
    GBuffer() = default;
    GBuffer(int width, int height);
    ~GBuffer() = default;

    void Init(int width, int height);
    void Deinit();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetPositionTexture() const { return m_PositionTexture; }
    inline unsigned int GetNormalTexture() const { return m_NormalTexture; }
    inline unsigned int GetAlbedoTexture() const { return m_AlbedoTexture; }

    inline unsigned int GetFBO() const { return m_FBO; }
    inline unsigned int GetRBO() const { return m_RBO; }

    void Resize(int width, int height);

private:
    void CheckStatus();

    unsigned int m_FBO;
    unsigned int m_RBO;

    // maps packed to reduce memory usage
    unsigned int m_PositionTexture; // .rgb = position, .a = roughness
    unsigned int m_NormalTexture; // .rgb = normal, .a = metallic
    unsigned int m_AlbedoTexture; // .rgb = albedo, .a = ambient occlusion

    uint32_t m_ResizeCallbackID = std::numeric_limits<uint32_t>::max();
};