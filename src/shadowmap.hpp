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

    inline unsigned int GetShadowMap() const { return m_ShadowMap; }

private:
    unsigned int m_FBO;
    unsigned int m_ShadowMap;
};

class PointLightShadowMap{
public:
    PointLightShadowMap() = default;
    ~PointLightShadowMap() = default;

    void Init();
    void Deinit();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetShadowMap() const { return m_ShadowMap; }

private:
    unsigned int m_FBO;
    unsigned int m_ShadowMap;
};