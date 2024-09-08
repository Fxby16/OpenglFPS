#pragma once

#include <limits>

class GPUBuffer{
public:
    GPUBuffer(unsigned int num_vertices, unsigned int vertex_size, unsigned int num_indices);
    GPUBuffer() = default;
    ~GPUBuffer() = default;

    void Init(unsigned int num_vertices, unsigned int vertex_size, unsigned int num_indices);
    void Free();

    void SetData(unsigned int start_index, const void* data, unsigned int num_vertices, unsigned int vertex_size) const;
    void SetIndices(const unsigned int* indices, unsigned int num_indices) const;

    void AddAttribute(unsigned int num_elements, unsigned int type, unsigned int stride);
    void AddAttributes(unsigned int* num_elements, unsigned int* types, unsigned int stride, unsigned int num_attributes);
    
    void BindVBO() const;
    void UnbindVBO() const;
    void BindEBO() const;
    void UnbindEBO() const;
    void BindVAO() const;
    void UnbindVAO() const;

private:
    unsigned int m_VBO = std::numeric_limits<unsigned int>::max();
    unsigned int m_EBO = std::numeric_limits<unsigned int>::max();
    unsigned int m_VAO = std::numeric_limits<unsigned int>::max();

    // vertex array variables
    unsigned int m_Offset = 0;
    unsigned int m_Index = 0;
};