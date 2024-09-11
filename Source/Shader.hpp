#pragma once

#include <unordered_map>
#include <string>
#include <glm.hpp>

class Shader{
public:
    Shader() = default;
    ~Shader() = default;

    void Load(const char* vertexPath, const char* fragmentPath);
    void Unload();

    void Reload();

    void Bind() const;
    void Unbind() const;
    
    inline int GetID() const { return m_ID; }

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1ui(const std::string& name, unsigned int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniformMat4fv(const std::string& name, const glm::mat4& matrix, unsigned int count = 1);
    void SetUniform1iv(const std::string& name, int* values, unsigned int count = 1);
    void SetUniform1iuv(const std::string& name, unsigned int* values, unsigned int count = 1);
    void SetUniform3fv(const std::string& name, const glm::vec3& vector, unsigned int count = 1);
    void SetUniform4fv(const std::string& name, const glm::vec4& vector, unsigned int count = 1);

private:
    int GetUniformLocation(const std::string& name);
    void Compile(const char* vertexCode, const char* fragmentCode);
    bool CheckCompileErrors(unsigned int shader_id);
    bool CheckLinkErrors();

    unsigned int m_ID;
    std::unordered_map<std::string, int> m_UniformsCache;
    std::string m_VertexPath;
    std::string m_FragmentPath;
};