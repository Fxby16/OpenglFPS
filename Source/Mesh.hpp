#pragma once

#include <Frustum.hpp>
#include <GPUBuffer.hpp>
#include <Texture.hpp>
#include <Shader.hpp>
#include <BoundingBox.hpp>
#include <Material.hpp>

#include <vector>
#include <glm.hpp>

inline constexpr unsigned int MAX_BONE_INFLUENCE = 4;

enum TextureType{
    ALBEDO,
    NORMAL,
    ROUGHNESS,
    METALLIC,
    AO,
    NUM_TEXTURE_TYPES
};

struct Vertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];
};

class Mesh{
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<uint32_t>& textures, const AABB& aabb);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const AABB& aabb);
    ~Mesh() = default;

    void InitMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<uint32_t>& textures, const AABB& aabb);
    void InitMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const AABB& aabb);
    void Free();

    void SetMaterial(const Material& material);

    void Draw(Shader& shader, glm::mat4 view, glm::mat4 model) const;
    void DrawShadows(Shader& shader, glm::mat4 light_space_matrix, glm::mat4 model) const;

    inline const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    inline const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    inline const std::vector<uint32_t>& GetTextures() const { return m_Textures; }
    inline const AABB& GetAABB() const { return m_AABB; }

    inline void SetVertices(const std::vector<Vertex>& vertices) { m_Vertices = vertices; }
    inline void SetIndices(const std::vector<unsigned int>& indices) { m_Indices = indices; }
    inline void SetTextures(const std::vector<uint32_t>& textures) { m_Textures = textures; }
    inline void SetAABB(const AABB& aabb) { m_AABB = aabb; }

    inline void SetHasTexture(int index, bool value) { m_HasTexture[index] = value; }

private:

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<uint32_t> m_Textures;
    GPUBuffer m_GPUBuffer;
    bool m_HasTexture[NUM_TEXTURE_TYPES] = {false};

    AABB m_AABB;
};