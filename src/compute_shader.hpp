#pragma once

class ComputeShader{
public:
    ComputeShader() = default;
    ~ComputeShader() = default;

    void Load(const char* computeShaderPath);
    void Unload();

    void Bind();
    void Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ);

    inline unsigned int GetID() const { return m_ID; }

private:
    unsigned int m_ID;
};