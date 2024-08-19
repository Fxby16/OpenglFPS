#include <predefined_meshes.hpp>

Mesh CUBE_MESH;
Mesh SPHERE_MESH;

void InitPredefinedMeshes()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Cube
    vertices = {
        // Face 1 (front)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},

        // Face 2 (back)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},

        // Face 3 (left)
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},

        // Face 4 (right)
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},

        // Face 5 (top)
        {{-0.5f, 0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},

        // Face 6 (bottom)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}}
    };

    // Use the same indices array as before
    indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    CUBE_MESH.InitMesh(vertices, indices, AABB(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f)));

    vertices.clear();
    indices.clear();
    
    // Sphere
    for(int y = 0; y <= SPHERE_SEGMENTS; y++){
        for(int x = 0; x <= SPHERE_SEGMENTS; x++){
            float xSegment = (float) x / (float) SPHERE_SEGMENTS;
            float ySegment = (float) y / (float) SPHERE_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            glm::vec3 normal = glm::vec3(xPos, yPos, zPos);
            glm::vec2 texCoords = glm::vec2(xSegment, ySegment);

            // Calculate tangent and bitangent vectors
            glm::vec3 tangent = glm::normalize(glm::vec3(-std::sin(xSegment * 2.0f * PI), 0.0f, std::cos(xSegment * 2.0f * PI)));
            glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

            vertices.push_back({normal, normal, texCoords, tangent, bitangent, {0, 0, 0, 0}, {0.0f, 0.0f, 0.0f, 0.0f}});
        }
    }

    for(int y = 0; y < SPHERE_SEGMENTS; y++){
        for(int x = 0; x < SPHERE_SEGMENTS; x++){
            int i0 = y * (SPHERE_SEGMENTS + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (SPHERE_SEGMENTS + 1);
            int i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    SPHERE_MESH.InitMesh(vertices, indices, AABB(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));     
}

void DeinitPredefinedMeshes()
{
    CUBE_MESH.Free();
    SPHERE_MESH.Free();
}