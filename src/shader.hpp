#pragma once

#include <raylib.h>
#include <unordered_map>
#include <string>

class ShaderEx{
public:
    ShaderEx() = default;
    ~ShaderEx() = default;

    void Load(const char* vertexPath, const char* fragmentPath);
    void Unload();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int value);
    void SetUniformMat4fv(const std::string& name, const Matrix& matrix, unsigned int count = 1);
    void SetUniform3fv(const std::string& name, const Vector3& vector);

    inline int GetID() const { return m_Shader.id; }
    inline Shader& GetShader() { return m_Shader; }

private:
    int GetUniformLocation(const std::string& name);

    Shader m_Shader;
    std::unordered_map<std::string, int> m_UniformsCache;
};