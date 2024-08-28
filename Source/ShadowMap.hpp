#pragma once

constexpr unsigned int SHADOWMAP_SIZE = 1024;

class ShadowMap{
public:
    ShadowMap() = default;
    ~ShadowMap() = default;

    void Init();
    void Deinit();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetShadowMapIndex() const { return m_ShadowMapIndex; }

private:
    unsigned int m_FBO;
    int m_ShadowMapIndex = -1;
};

class PointLightShadowMap{
public:
    PointLightShadowMap() = default;
    ~PointLightShadowMap() = default;

    void Init();
    void Deinit();

    void Bind(unsigned int face) const;
    void Unbind() const;

    inline unsigned int GetShadowMapIndex() const { return m_ShadowMapIndex; }

private:
    unsigned int m_FBO;
    int m_ShadowMapIndex = -1;
};

unsigned int InitShadowMapArray(unsigned int count);
unsigned int InitCubeShadowMapArray(unsigned int count);