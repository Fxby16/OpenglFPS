#include <compute_shader.hpp>
#include <log.hpp>

#include <glad/glad.h>

#include <cstdio>
#include <vector>

void ComputeShader::Load(const char* computeShaderPath)
{
    FILE* file = fopen(computeShaderPath, "r");

    if(!file){
        LogError("Failed to open file: %s", computeShaderPath);
        return;
    }

    std::vector<char> buffer;

    char ch;
    while((ch = fgetc(file)) != EOF){
        buffer.push_back(ch);
    }

    fclose(file);

    buffer.push_back('\0');

    const char* source = buffer.data();

    unsigned int id = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success){
        char infoLog[512];
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        LogError("Failed to compile compute shader: %s", infoLog);
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, id);
    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    if(!success){
        char infoLog[512];
        glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
        LogError("Failed to link compute shader: %s", infoLog);
    }

    glDeleteShader(id);
}

void ComputeShader::Unload()
{
    glDeleteProgram(m_ID);
}

void ComputeShader::Bind()
{
    glUseProgram(m_ID);
}

void ComputeShader::Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ)
{
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}