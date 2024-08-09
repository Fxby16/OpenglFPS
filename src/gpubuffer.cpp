#include <gpubuffer.hpp>

#include <glad/glad.h>
#include <cstdio>

unsigned int GetGlTypeSize(unsigned int type)
{
    switch(type){
        case GL_FLOAT:
            return sizeof(float);
        case GL_INT:
            return sizeof(int);
        case GL_UNSIGNED_INT:
            return sizeof(unsigned int);
        default:
            printf("Unknown type: %d\n", type);
            return 0;
    }
}

GPUBuffer::GPUBuffer(unsigned int num_vertices, unsigned int vertex_size, unsigned int num_indices)
{
    Init(num_vertices, vertex_size, num_indices);
}

void GPUBuffer::Init(unsigned int num_vertices, unsigned int vertex_size, unsigned int num_indices)
{
    #ifdef DEBUG
        printf("GPUBuffer::Init");
    #endif

    if(num_vertices > 0){
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER ,m_VBO);
        glBufferData(GL_ARRAY_BUFFER, num_vertices * vertex_size, nullptr, GL_DYNAMIC_DRAW); 
    }

    glGenVertexArrays(1,&m_VAO);

    if(num_indices > 0){
        glGenBuffers(1, &m_EBO);   
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW); 
    }

    #ifdef DEBUG
        if(num_vertices > 0){
            printf(" VBO: %d", m_VBO);
        }
        printf(" VAO: %d", m_VAO);
        if(num_indices > 0){
            printf(" EBO: %d", m_EBO);
        }
        printf("\n");
    #endif

    UnbindVBO();
    UnbindEBO();
    UnbindVAO();
}

void GPUBuffer::Free()
{
    #ifdef DEBUG
        printf("GPUBuffer::Free()");
        if(m_VBO != std::numeric_limits<unsigned int>::max()){
            printf(" VBO: %d", m_VBO);
        }
        printf(" VAO: %d", m_VAO);
        if(m_EBO != std::numeric_limits<unsigned int>::max()){
            printf(" EBO: %d", m_EBO);
        }
        printf("\n");
    #endif

    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void GPUBuffer::SetData(unsigned int start_index, const float* data, unsigned int num_vertices, unsigned int vertex_size) const
{
    BindVBO();
    glBufferSubData(GL_ARRAY_BUFFER, start_index, num_vertices * vertex_size, data);
}

void GPUBuffer::SetIndices(const unsigned int* indices, unsigned int num_indices) const
{
    BindEBO();
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, num_indices * sizeof(unsigned int), indices);
}

void GPUBuffer::AddAttribute(unsigned int num_elements, unsigned int type, unsigned int stride)
{
    BindVAO();
    glEnableVertexAttribArray(m_Index);
    if(type == GL_FLOAT){
        glVertexAttribPointer(m_Index, num_elements, type, GL_FALSE, stride, (void*)m_Offset);
    }else if(type == GL_INT || type == GL_UNSIGNED_INT){
        glVertexAttribIPointer(m_Index, num_elements, type, stride, (void*)m_Offset);
    }
    m_Offset += num_elements * GetGlTypeSize(type);
    m_Index++;
}

void GPUBuffer::AddAttributes(unsigned int* num_elements, unsigned int* types, unsigned int stride, unsigned int num_attributes)
{
    BindVAO();
    for(unsigned int i = 0; i < num_attributes; i++){
        glEnableVertexAttribArray(m_Index);
        glVertexAttribPointer(m_Index, num_elements[i], types[i], GL_FALSE, stride, (void*)m_Offset);
        m_Offset += num_elements[i] * GetGlTypeSize(types[i]);
        m_Index++;
    }
}

void GPUBuffer::BindVBO() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

void GPUBuffer::UnbindVBO() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUBuffer::BindEBO() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}

void GPUBuffer::UnbindEBO() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GPUBuffer::BindVAO() const
{
    glBindVertexArray(m_VAO);
}

void GPUBuffer::UnbindVAO() const
{
    glBindVertexArray(0);
}