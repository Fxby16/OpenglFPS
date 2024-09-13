#include <Skydome.hpp>
#include <Log.hpp>
#include <Shader.hpp>
#include <ResourceManager.hpp>

#include <stb_image.h>
#include <glad/glad.h>
#include <vector>
#include <cmath>

static std::vector<float> vertices;
static std::vector<unsigned int> indices;
static unsigned int skydomeVAO;
static unsigned int skydomeVBO;
static unsigned int skydomeEBO;
static uint32_t skydomeShader;
static unsigned int skydomeTexture;
static constexpr unsigned int stacks = 18; // on the y-axis
static constexpr unsigned int slices = 36; // on the x-z plane
static constexpr float radius = 50.0f;

void GenerateDome()
{
    const float PI = 3.14159265359f;

    // Clear the vertices and indices vectors
    vertices.clear();
    indices.clear();

    // Generate vertices
    for (unsigned int i = 0; i <= stacks; ++i) {
        float stackAngle = PI / 2.0f - (float)i / stacks * (PI / 2.0f);  // from PI/2 to 0 for hemisphere
        float xy = radius * cosf(stackAngle);  // radius * cos(stackAngle)
        float y = radius * sinf(stackAngle);   // radius * sin(stackAngle)

        // Generate vertices along each slice
        for (unsigned int j = 0; j <= slices; ++j) {
            float sliceAngle = (float)j / slices * 2.0f * PI;  // from 0 to 2PI

            // Calculate vertex position (x, y, z)
            float x = xy * cosf(sliceAngle);
            float z = xy * sinf(sliceAngle);

            // Push back vertex position (x, y, z)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Generate indices for GL_TRIANGLES
    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            unsigned int first = i * (slices + 1) + j;         // Current vertex
            unsigned int second = (i + 1) * (slices + 1) + j;  // Vertex directly below current

            // Two triangles per quad
            indices.push_back(first);        // First triangle
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);       // Second triangle
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void LoadSkydome(const std::string& hdri)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(hdri.c_str(), &width, &height, &channels, 0);

    if(!data){
        LogError("Failed to load HDRI image: %s. %s", hdri.c_str(), stbi_failure_reason());
        return;
    }

    glGenTextures(1, &skydomeTexture);
    glBindTexture(GL_TEXTURE_2D, skydomeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    GenerateDome();

    glGenVertexArrays(1, &skydomeVAO);
    glGenBuffers(1, &skydomeVBO);
    glGenBuffers(1, &skydomeEBO);

    glBindVertexArray(skydomeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skydomeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skydomeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    skydomeShader = LoadShader("Resources/Shaders/Skydome.vert", "Resources/Shaders/Skydome.frag");
}

void UnloadSkydome()
{
    glDeleteTextures(1, &skydomeTexture);
    glDeleteVertexArrays(1, &skydomeVAO);
    glDeleteBuffers(1, &skydomeVBO);
    glDeleteBuffers(1, &skydomeEBO);
}

void DrawSkydome(glm::mat4 view, glm::mat4 projection, glm::mat4 model)
{
    Shader& shader = *GetShader(skydomeShader);
    shader.Bind();
    shader.SetUniformMat4fv("view", view);
    shader.SetUniformMat4fv("projection", projection);
    shader.SetUniformMat4fv("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skydomeTexture);

    glBindVertexArray(skydomeVAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    shader.Unbind();
}