#include <shader.hpp>
#include <glad.h>
#include <raymath.h>

void ShaderEx::Load(const char* vertexPath, const char* fragmentPath)
{
    m_Shader = LoadShader(vertexPath, fragmentPath);
}

void ShaderEx::Unload()
{
    UnloadShader(m_Shader);
}

void ShaderEx::Bind() const
{
    glUseProgram(m_Shader.id);
}

void ShaderEx::Unbind() const
{
    glUseProgram(0);
}

void ShaderEx::SetUniform1i(const std::string& name, int value)
{
    int location = GetUniformLocation(name);
    glUniform1i(location, value);
}

void ShaderEx::SetUniformMat4fv(const std::string& name, const Matrix& matrix, unsigned int count)
{
    int location = GetUniformLocation(name);
    glUniformMatrix4fv(location, count, GL_FALSE, MatrixToFloat(matrix));
}

void ShaderEx::SetUniform3fv(const std::string& name, const Vector3& vector)
{
    int location = GetUniformLocation(name);
    glUniform3fv(location, 1, Vector3ToFloat(vector));
}

int ShaderEx::GetUniformLocation(const std::string& name)
{
    if(m_UniformsCache.find(name) != m_UniformsCache.end()){
        return m_UniformsCache[name];
    }

    int location = GetShaderLocation(m_Shader, name.c_str());
    if(location == -1){
        TraceLog(LOG_WARNING, "Uniform %s not found in shader", name.c_str());
    }else{
        m_UniformsCache[name] = location;
    }

    return location;
}