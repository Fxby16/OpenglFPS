#include <mesh.hpp>
#include <globals.hpp>
#include <resource_manager.hpp>

#include <glad/glad.h>
#include <string>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<uint32_t>& textures, const BoundingBox& aabb)
{
    InitMesh(vertices, indices, textures, aabb);
}

void Mesh::InitMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<uint32_t>& textures, const BoundingBox& aabb)
{
    m_Vertices = vertices;
    m_Indices = indices;
    m_Textures = textures;
    m_AABB = aabb;

    m_GPUBuffer.Init(vertices.size(), sizeof(Vertex), indices.size());

    m_GPUBuffer.SetData(0, (float*)vertices.data(), vertices.size(), sizeof(Vertex));
    m_GPUBuffer.SetIndices(indices.data(), indices.size());

    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(2, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(4, GL_INT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(4, GL_FLOAT, sizeof(Vertex));
}

void Mesh::InitMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const BoundingBox& aabb)
{
    m_Vertices = vertices;
    m_Indices = indices;
    m_AABB = aabb;

    m_GPUBuffer.Init(vertices.size(), sizeof(Vertex), indices.size());

    m_GPUBuffer.SetData(0, (float*)vertices.data(), vertices.size(), sizeof(Vertex));
    m_GPUBuffer.SetIndices(indices.data(), indices.size());

    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(2, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(3, GL_FLOAT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(4, GL_INT, sizeof(Vertex));
    m_GPUBuffer.AddAttribute(4, GL_FLOAT, sizeof(Vertex));
}

void Mesh::Free()
{
    m_GPUBuffer.Free();
}

void Mesh::SetMaterial(const Material& material)
{
    m_Textures = material.GetTextures();
}

void Mesh::Draw(Shader& shader, glm::mat4 view, glm::mat4 model, bool pbr, bool is_compressed) const
{
    //skip if the mesh is not visible
    BoundingBox transformed = m_AABB;
    //transform the bounding box to model space
    transformed.min = model * glm::vec4(m_AABB.min, 1.0f);
    transformed.max = model * glm::vec4(m_AABB.max, 1.0f);

    if(!AABBInFrustum(g_Frustum, transformed.min, transformed.max)){
        #ifdef DEBUG
            culled++;
        #endif
        return;
    } 

    #ifdef DEBUG
        drawn++;
    #endif
    
    shader.Bind();

    for(int i = 0; i < m_Textures.size(); i++){
        std::string number;
        std::string name = GetTexture(m_Textures[i]).GetType();

        if(name == "texture_diffuse"){
            shader.SetUniform1i("albedoMap", i);
        }else{
            shader.SetUniform1i(name.c_str(), i);
        }

        //printf("Binding texture: %s\n", name.c_str());

        GetTexture(m_Textures[i]).Bind(i);
    }

    m_GPUBuffer.BindVAO();
    m_GPUBuffer.BindEBO();
    m_GPUBuffer.BindVBO();

    shader.SetUniformMat4fv("view", view, 1);
    shader.SetUniformMat4fv("model", model, 1);

    shader.SetUniform1i("usePBR", pbr);

    shader.SetUniform1i("isCompressed", is_compressed);
    
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::DrawShadows(Shader& shader, glm::mat4 light_space_matrix, glm::mat4 model) const
{
    shader.Bind();

    m_GPUBuffer.BindVAO();
    m_GPUBuffer.BindEBO();
    m_GPUBuffer.BindVBO();

    shader.SetUniformMat4fv("lightSpaceMatrix", light_space_matrix, 1);
    shader.SetUniformMat4fv("model", model, 1);

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
}