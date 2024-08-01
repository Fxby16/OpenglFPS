#pragma once

#include <frustum.hpp>
#include <gpubuffer.hpp>
#include <texture.hpp>
#include <shader.hpp>

#include <vector>
#include <raylib.h>

inline constexpr unsigned int MAX_BONE_INFLUENCE = 4;

struct Vertex{
    Vector3 Position;
    Vector3 Normal;
    Vector2 TexCoords;
    Vector3 Tangent;
    Vector3 Bitangent;

    int BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];
};

class MeshEx{
public:
    MeshEx() = default;
    MeshEx(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureEx>& textures, const BoundingBox& aabb);
    ~MeshEx() = default;

    void InitMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureEx>& textures, const BoundingBox& aabb);
    void Free();

    void Draw(ShaderEx& shader, Matrix view, Matrix model, bool pbr) const;

    inline const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    inline const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    inline const std::vector<TextureEx>& GetTextures() const { return m_Textures; }

private:

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<TextureEx> m_Textures;
    GPUBuffer m_GPUBuffer;

    BoundingBox m_AABB;
};