#include <PredefinedMeshes.hpp>

Mesh CUBE_MESH;
Mesh SPHERE_MESH;

void InitPredefinedMeshes()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Cube
    vertices = {
        // Front face
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),

        // Back face
        Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),

        // Left face
        Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),

        // Right face
        Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),

        // Top face
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),

        // Bottom face
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f})
    };

    indices = {
        0, 3, 2,  // Front face
        2, 1, 0,  // Front face
    
        4, 5, 6,  // Back face
        6, 7, 4,  // Back face
    
        8, 9, 10, // Left face
        10, 11, 8, // Left face
    
        12, 15, 14, // Right face
        14, 13, 12, // Right face
    
        16, 19, 18, // Top face
        18, 17, 16, // Top face
    
        20, 21, 22, // Bottom face
        22, 23, 20  // Bottom face
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

            vertices.push_back(Vertex(normal, normal, texCoords, tangent, (int[]){-1, -1, -1, -1}, (float[]){0.0f, 0.0f, 0.0f, 0.0f}));
        }
    }

    for(int y = 0; y < SPHERE_SEGMENTS; y++){
        for(int x = 0; x < SPHERE_SEGMENTS; x++){
            int i0 = y * (SPHERE_SEGMENTS + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (SPHERE_SEGMENTS + 1);
            int i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    SPHERE_MESH.InitMesh(vertices, indices, AABB(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));     
}

void DeinitPredefinedMeshes()
{
    CUBE_MESH.Free();
    SPHERE_MESH.Free();
}