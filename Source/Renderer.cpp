#include <Renderer.hpp>
#include <Globals.hpp>
#include <OpenGL.hpp>
#include <Shader.hpp>
#include <Camera.hpp>
#include <GPUBuffer.hpp>

static GPUBuffer g_FullscreenQuadBuffer;
static GPUBuffer g_TextureBuffer;
static GPUBuffer g_LineBuffer;
static GPUBuffer g_CubeBuffer;
static Framebuffer g_DeferredPassFramebuffer;

static GBuffer g_GBuffer;
static DeferredMode g_DeferredMode = DEFERRED_SHADING;

static Shader g_SolidShapeShader;
static Shader g_TextureShader;

static glm::mat4 g_Proj = glm::mat4(1.0f);
static glm::mat4 g_View = glm::mat4(1.0f);   
static glm::mat4 g_Model = glm::mat4(1.0f); 

GBuffer& GetGBuffer()
{
    return g_GBuffer;
}

DeferredMode GetDeferredMode()
{
    return g_DeferredMode;
}

void SetDeferredMode(DeferredMode mode)
{
    g_DeferredMode = mode;
}

void InitRenderer()
{
    float fullscreenQuadData[] = {
        // Vertex positions    // Texture coordinates
        -1.0f,  1.0f,          0.0f, 1.0f,  // Top-left
        -1.0f, -1.0f,          0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f,          1.0f, 0.0f,  // Bottom-right

        -1.0f,  1.0f,          0.0f, 1.0f,  // Top-left
         1.0f, -1.0f,          1.0f, 0.0f,  // Bottom-right
         1.0f,  1.0f,          1.0f, 1.0f   // Top-right
    };

    float cubeData[] = {
        // Front face
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,

        // Back face
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        // Left face
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // Right face
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,

        // Top face
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f
    };

    unsigned int cubeIndices[] = {
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

    g_FullscreenQuadBuffer.Init(6, 4 * sizeof(float), 0);
    g_FullscreenQuadBuffer.SetData(0, (void*) fullscreenQuadData, 6, 4 * sizeof(float));
    g_FullscreenQuadBuffer.AddAttribute(2, GL_FLOAT, 4 * sizeof(float));
    g_FullscreenQuadBuffer.AddAttribute(2, GL_FLOAT, 4 * sizeof(float));

    g_TextureShader.Load("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    g_TextureBuffer.Init(6, 4 * sizeof(float), 0);
    g_TextureBuffer.BindVBO();
    g_TextureBuffer.AddAttribute(2, GL_FLOAT, 4 * sizeof(float));
    g_TextureBuffer.AddAttribute(2, GL_FLOAT, 4 * sizeof(float));

    g_SolidShapeShader.Load("Resources/Shaders/SolidShape.vert", "Resources/Shaders/SolidShape.frag");
    g_LineBuffer.Init(2, 3 * sizeof(float), 0);
    g_LineBuffer.BindVBO();
    g_LineBuffer.AddAttribute(3, GL_FLOAT, 3 * sizeof(float));

    g_CubeBuffer.Init(24, 3 * sizeof(float), 36);
    g_CubeBuffer.SetData(0, (void*) cubeData, 24, 3 * sizeof(float));
    g_CubeBuffer.SetIndices(cubeIndices, 36);
    g_CubeBuffer.AddAttribute(3, GL_FLOAT, 3 * sizeof(float));

    g_DeferredPassFramebuffer.Init(g_ScreenWidth, g_ScreenHeight);
    g_GBuffer.Init(g_ScreenWidth, g_ScreenHeight);
}

void DeinitRenderer()
{
    g_FullscreenQuadBuffer.Free();
    g_TextureBuffer.Free();
    g_LineBuffer.Free();
    g_TextureShader.Unload();
    g_SolidShapeShader.Unload();

    g_DeferredPassFramebuffer.Deinit();
}

void UpdateProj(glm::mat4 proj)
{
    g_Proj = proj;
}

void UpdateView(glm::mat4 view)
{
    g_View = view;
}

void DeferredPass(GBuffer& gBuffer, Shader& deferredShader, Camera& camera)
{
    switch(g_DeferredMode){
        case DEFERRED_SHADING:
        {
            g_DeferredPassFramebuffer.Bind();
            ClearScreen();

            DisableColorBlend();
            DisableDepthTest();
            
            deferredShader.Bind();
            BindTexture(gBuffer.GetPositionTexture(), 0);
            BindTexture(gBuffer.GetNormalTexture(), 1);
            BindTexture(gBuffer.GetAlbedoTexture(), 2);

            DrawFullscreenQuad();
            
            EnableColorBlend();
            EnableDepthTest();

            // As a last step, we now copy over the depth buffer from our g-buffer to the default framebuffer.
            BindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetFBO());
            BindFramebuffer(GL_DRAW_FRAMEBUFFER, g_DeferredPassFramebuffer.GetFBO());
            BlitFramebuffer(0, 0, g_ScreenWidth, g_ScreenHeight, 0, 0, g_ScreenWidth, g_ScreenHeight, GL_DEPTH_BUFFER_BIT);
        } break;
        case DEFERRED_POSITION:
        {
            UnbindFramebuffer();
            DisableColorBlend();
            DisableDepthTest();
            DrawTexture(gBuffer.GetPositionTexture(), -1.0f, -1.0f, 2.0f, 2.0f);
            EnableColorBlend();
            EnableDepthTest();
        } break;
        case DEFERRED_NORMAL:
        {
            UnbindFramebuffer();
            DisableColorBlend();
            DisableDepthTest();
            DrawTexture(gBuffer.GetNormalTexture(), -1.0f, -1.0f, 2.0f, 2.0f);    
            EnableColorBlend();
            EnableDepthTest();
        } break;
        case DEFERRED_ALBEDO:
        {
            UnbindFramebuffer();
            DisableColorBlend();
            DisableDepthTest();
            DrawTexture(gBuffer.GetAlbedoTexture(), -1.0f, -1.0f, 2.0f, 2.0f);    
            EnableColorBlend();
            EnableDepthTest();
        } break;
        default: break;
    }
}

void DrawFullscreenQuad()
{
    g_FullscreenQuadBuffer.BindVAO();
    g_FullscreenQuadBuffer.BindVBO();

    glDrawArrays(GL_TRIANGLES, 0, 6);
}   

Framebuffer& GetDeferredPassFramebuffer()
{
    return g_DeferredPassFramebuffer;
}

void DrawLine3D(glm::vec3 start, glm::vec3 end, glm::vec4 color)
{
    g_SolidShapeShader.Bind();
    g_SolidShapeShader.SetUniform4fv("color", color, 1);
    g_SolidShapeShader.SetUniformMat4fv("proj", g_Proj, 1);
    g_SolidShapeShader.SetUniformMat4fv("view", g_View, 1);
    g_SolidShapeShader.SetUniformMat4fv("model", glm::mat4(1.0f), 1);

    float data[] = {
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };

    g_LineBuffer.SetData(0, (void*) data, 2, 3 * sizeof(float));
    g_LineBuffer.BindVAO();
    g_LineBuffer.BindVBO();

    glDrawArrays(GL_LINES, 0, 2);
}

void DrawSolidCube(glm::vec3 position, glm::vec3 scale, glm::vec4 color)
{
    g_SolidShapeShader.Bind();
    g_SolidShapeShader.SetUniform4fv("color", color, 1);
    g_SolidShapeShader.SetUniformMat4fv("proj", g_Proj, 1);
    g_SolidShapeShader.SetUniformMat4fv("view", g_View, 1);
    g_SolidShapeShader.SetUniformMat4fv("model", glm::translate(g_Model, position) * glm::scale(g_Model, scale), 1);

    g_CubeBuffer.BindVAO();
    g_CubeBuffer.BindVBO();
    g_CubeBuffer.BindEBO();

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void DrawBoundingBox(glm::vec3 min, glm::vec3 max, glm::vec4 color)
{
    glm::vec3 vertices[] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, max.z),
        glm::vec3(min.x, max.y, max.z)
    };

    DrawLine3D(vertices[0], vertices[1], color);
    DrawLine3D(vertices[1], vertices[2], color);
    DrawLine3D(vertices[2], vertices[3], color);
    DrawLine3D(vertices[3], vertices[0], color);

    DrawLine3D(vertices[4], vertices[5], color);
    DrawLine3D(vertices[5], vertices[6], color);
    DrawLine3D(vertices[6], vertices[7], color);
    DrawLine3D(vertices[7], vertices[4], color);

    DrawLine3D(vertices[0], vertices[4], color);
    DrawLine3D(vertices[1], vertices[5], color);
    DrawLine3D(vertices[2], vertices[6], color);
    DrawLine3D(vertices[3], vertices[7], color);
}

void DrawBoundingBox(const AABB& box, glm::vec4 color)
{
    DrawBoundingBox(box.min, box.max, color);
}

void DrawTexture(unsigned int texture, float x, float y, float width, float height)
{
    g_TextureBuffer.BindVAO();
    g_TextureBuffer.BindVBO();
    g_TextureShader.Bind();    

    BindTexture(texture, 0);

    float data[] = {
        x, y + height, 0.0f, 1.0f,          // Top-left
        x, y, 0.0f, 0.0f,                   // Bottom-left
        x + width, y, 1.0f, 0.0f,           // Bottom-right

        x, y + height, 0.0f, 1.0f,          // Top-left
        x + width, y, 1.0f, 0.0f,           // Bottom-right
        x + width, y + height, 1.0f, 1.0f   // Top-right
    };

    g_TextureBuffer.SetData(0, (void*) data, 6, 4 * sizeof(float));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}