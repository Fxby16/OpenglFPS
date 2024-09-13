#pragma once

#include <string>
#include <unordered_map>

class ComputeShader{
public:
    ComputeShader() = default;
    ~ComputeShader() = default;

    void Load(const char* computeShaderPath);
    void Unload();

    void Bind();
    void Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ);

    inline unsigned int GetID() const { return m_ID; }

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform2i(const std::string& name, int x, int y);
    void SetUniform4f(const std::string& name, float x, float y, float z, float w);

private:
    int GetUniformLocation(const std::string& name);

    unsigned int m_ID;
    std::unordered_map<std::string, int> m_UniformsCache;
};