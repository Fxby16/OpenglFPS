#include <shader.hpp>
#include <log.hpp>
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

void Shader::Load(const char* vertexPath, const char* fragmentPath)
{
    m_ID = glCreateProgram();

    std::string vertex_src_code;
    std::string fragment_src_code;

    FILE* vertex_file = fopen(vertexPath, "r");
    
    if(!vertex_file){
        LogError("Could not open vertex shader file %s\n", vertexPath);
        return;
    }

    FILE* fragment_file = fopen(fragmentPath, "r");

    if(!fragment_file){
        LogError("Could not open fragment shader file %s\n", fragmentPath);
        return;
    }

    char ch;

    while((ch = fgetc(vertex_file)) != EOF){
        vertex_src_code += ch;
    }

    while((ch = fgetc(fragment_file)) != EOF){
        fragment_src_code += ch;
    }

    fclose(vertex_file);
    fclose(fragment_file);

    m_VertexPath = vertexPath;
    m_FragmentPath = fragmentPath;

    Compile(vertex_src_code.c_str(), fragment_src_code.c_str());

    #ifdef DEBUG
        LogMessage("Shader %s and %s loaded successfully\n", vertexPath, fragmentPath);
    #endif
}

void Shader::Compile(const char* vertex_src_code, const char* fragment_src_code)
{
    unsigned int vertex_shader, fragment_shader;
    bool status;

    //compile vertex Shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_src_code, nullptr);
    glCompileShader(vertex_shader);
    status = CheckCompileErrors(vertex_shader);

    if(!status){
        LogError("Couldn't compile shader %s\n", m_VertexPath.c_str());
    }

    //compile fragment Shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_src_code, nullptr);
    glCompileShader(fragment_shader);
    status = CheckCompileErrors(fragment_shader);
    
    if(!status){
        LogError("Couldn't compile shader %s\n", m_FragmentPath.c_str());
    }

    glAttachShader(m_ID, vertex_shader);
    glAttachShader(m_ID, fragment_shader);
    glLinkProgram(m_ID);

    status = CheckLinkErrors();

    if(!status){
        LogError("Couldn't link shaders %s and %s\n", m_VertexPath.c_str(), m_FragmentPath.c_str());
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

bool Shader::CheckCompileErrors(unsigned int shader_id)
{
    int compile_status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);

    if(compile_status != GL_TRUE){
        int info_log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        
        char* buffer = new char[info_log_length];

        int buffer_size;
        glGetShaderInfoLog(shader_id, info_log_length, &buffer_size, buffer);

        LogError("%s\n", buffer);

        delete[] buffer;

        return false;
    }
    return true;
}

bool Shader::CheckLinkErrors()
{
    int link_status;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &link_status);

    if(link_status != GL_TRUE){
        int info_log_length;
        glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &info_log_length);
        
        char* buffer = new char[info_log_length];

        int buffer_size;
        glGetProgramInfoLog(m_ID, info_log_length, &buffer_size, buffer);

        LogError("%s\n", buffer);

        delete[] buffer;

        return false;
    }

    return true;
}

void Shader::Unload()
{
    glDeleteProgram(m_ID);
}

void Shader::Bind() const
{
    glUseProgram(m_ID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    int location = GetUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    int location = GetUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::SetUniformMat4fv(const std::string& name, const glm::mat4& matrix, unsigned int count)
{
    int location = GetUniformLocation(name);
    glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform1iv(const std::string& name, int* values, unsigned int count)
{
    int location = GetUniformLocation(name);
    glUniform1iv(location, count, values);
}

void Shader::SetUniform1iuv(const std::string& name, unsigned int* values, unsigned int count)
{
    int location = GetUniformLocation(name);
    glUniform1uiv(location, count, values);
}

void Shader::SetUniform3fv(const std::string& name, const glm::vec3& vector, unsigned int count)
{
    int location = GetUniformLocation(name);
    glUniform3fv(location, count, glm::value_ptr(vector));
}

void Shader::SetUniform4fv(const std::string& name, const glm::vec4& vector, unsigned int count)
{
    int location = GetUniformLocation(name);
    glUniform4fv(location, count, glm::value_ptr(vector));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if(m_UniformsCache.find(name) != m_UniformsCache.end()){
        return m_UniformsCache[name];
    }

    int location = glGetUniformLocation(m_ID, name.c_str());
    if(location == -1){
        LogWarning("Uniform %s not found in shader", name.c_str());
    }else{
        m_UniformsCache[name] = location;
    }

    return location;
}