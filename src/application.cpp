#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include <application.hpp>
#include <pbr.hpp>

#include <stdio.h>

#include <glad.h>

void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(type == GL_DEBUG_TYPE_ERROR){
        TraceLog(LOG_ERROR, "OpenGL error: %s (source: %d, type: %d, id: %d, severity: %d)", message, source, type, id, severity);
    }
}

Application::Application()
{
    Init();
}

Application::~Application()
{
    Deinit();
}

void Application::Init()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitWindow(g_ScreenWidth, g_ScreenHeight, g_WindowTitle);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGLErrorCallback, 0);

    DisableCursor();

    m_Camera.position = (Vector3){ 5.0f, 4.0f, 5.0f };    // Camera position
    m_Camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };      // Camera looking at point
    m_Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_Camera.fovy = 60.0f;                                // Camera field-of-view Y
    m_Camera.projection = CAMERA_PERSPECTIVE;

    m_Model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
    m_Cube = LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f));
    m_rk62 = LoadModel("resources/models/rk62/scene.gltf");

    m_GBufferShader = LoadShader("resources/shaders/gbuffer.vs",
                               "resources/shaders/gbuffer.fs");

    m_DeferredShader = LoadShader("resources/shaders/deferred_shading.vs",
                               "resources/shaders/deferred_shading.fs");
    m_DeferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(m_DeferredShader, "viewPosition");

    m_GBuffer.framebuffer = rlLoadFramebuffer();

    if(!m_GBuffer.framebuffer){
        TraceLog(LOG_WARNING, "Failed to create framebuffer");
    }

    rlEnableFramebuffer(m_GBuffer.framebuffer);

    // floating point textures to store g-buffer data
    // albedo.rgb = albedo map, albedo.a = ambient occlusion map
    // normal.rgb = normal map, normal.a = metallic map
    // position.rgb = position, position.a = roughness
    m_GBuffer.positionTexture = rlLoadTexture(nullptr, g_ScreenWidth, g_ScreenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1);
    m_GBuffer.normalTexture = rlLoadTexture(nullptr, g_ScreenWidth, g_ScreenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1);
    m_GBuffer.albedoSpecTexture = rlLoadTexture(nullptr, g_ScreenWidth, g_ScreenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1);

    rlActiveDrawBuffers(3);

    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.positionTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.normalTexture, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.albedoSpecTexture, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);

    m_GBuffer.depthRenderbuffer = rlLoadTextureDepth(g_ScreenWidth, g_ScreenHeight, true);
    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.depthRenderbuffer, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

    if (!rlFramebufferComplete(m_GBuffer.framebuffer)){
        TraceLog(LOG_WARNING, "Framebuffer is not complete");
    }

    for(int i = 0; i < m_rk62.materialCount; i++){
        m_rk62.materials[i].shader = m_GBufferShader;
    }

    /*for (int i = 0; i < MAX_CUBES; i++){
        m_CubePositions[i] = (Vector3){
            .x = (float)(rand()%10) - 5,
            .y = (float)(rand()%5),
            .z = (float)(rand()%10) - 5,
        };

        m_CubeRotations[i] = (float)(rand()%360);
    }*/

    SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "Positions"), (int[1]){0}, SHADER_UNIFORM_INT);
    SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "Normals"), (int[1]){1}, SHADER_UNIFORM_INT);
    SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "Albedo"), (int[1]){2}, SHADER_UNIFORM_INT);

    rlEnableDepthTest();

    SetTargetFPS(60);
}

void Application::Deinit()
{
    UnloadModel(m_Model);
    UnloadModel(m_rk62);
    UnloadModel(m_Cube);

    UnloadShader(m_DeferredShader);
    UnloadShader(m_GBufferShader);

    rlUnloadFramebuffer(m_GBuffer.framebuffer);
    rlUnloadTexture(m_GBuffer.positionTexture);
    rlUnloadTexture(m_GBuffer.normalTexture);
    rlUnloadTexture(m_GBuffer.albedoSpecTexture);
    rlUnloadTexture(m_GBuffer.depthRenderbuffer);

    CloseWindow();
}

void Application::Run()
{
    SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "numLights"), (int[1]){1}, SHADER_UNIFORM_INT);
    SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "lightColor[0]"), (float[3]){10.0f, 10.0f, 10.0f}, SHADER_UNIFORM_VEC3);

    while (!WindowShouldClose())
    {
        rlEnableShader(m_DeferredShader.id);
            SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "camPos"), &m_Camera.position, SHADER_UNIFORM_VEC3);
            SetShaderValue(m_DeferredShader, GetShaderLocation(m_DeferredShader, "lightPos[0]"), &m_Camera.position, SHADER_UNIFORM_VEC3);
        rlDisableShader();

        HandleInputs();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            rlEnableFramebuffer(m_GBuffer.framebuffer);
            rlClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            rlClearScreenBuffers();

            rlDisableColorBlend();
            BeginMode3D(m_Camera);

                rlEnableShader(m_GBufferShader.id);

                    SetShaderValue(m_GBufferShader, GetShaderLocation(m_GBufferShader, "usePBR"), (int[1]){1}, SHADER_UNIFORM_INT);
                    DrawModelPBR(m_rk62, Vector3Zero(), 0.05f, WHITE);

                    //usePBR = 0;
                    //rlSetUniform(GetShaderLocation(m_GBufferShader, "usePBR"), &usePBR, SHADER_UNIFORM_INT, 1);
                    //for (int i = 0; i < MAX_CUBES; i++){
                    //    Vector3 position = m_CubePositions[i];
                    //    DrawModelEx(m_Cube, position, (Vector3) { 1, 1, 1 }, m_CubeRotations[i],
                    //        (Vector3) { m_CubeScale, m_CubeScale, m_CubeScale }, WHITE);
                    //}

                rlDisableShader();
            EndMode3D();

            rlEnableColorBlend();

            rlDisableFramebuffer();
            rlClearScreenBuffers();

            switch (m_DeferredMode){
                case DEFERRED_SHADING:
                {
                    BeginMode3D(m_Camera);
                        rlDisableColorBlend();
                        rlDisableDepthTest();
                        rlEnableShader(m_DeferredShader.id);

                            rlActiveTextureSlot(0);
                            rlEnableTexture(m_GBuffer.positionTexture);
                            rlActiveTextureSlot(1);
                            rlEnableTexture(m_GBuffer.normalTexture);
                            rlActiveTextureSlot(2);
                            rlEnableTexture(m_GBuffer.albedoSpecTexture);

                            rlLoadDrawQuad();
                        rlDisableShader();
                        rlEnableDepthTest();
                        rlEnableColorBlend();
                    EndMode3D();

                    // As a last step, we now copy over the depth buffer from our g-buffer to the default framebuffer.
                    rlBindFramebuffer(RL_READ_FRAMEBUFFER, m_GBuffer.framebuffer);
                    rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, 0);
                    rlBlitFramebuffer(0, 0, g_ScreenWidth, g_ScreenHeight, 0, 0, g_ScreenWidth, g_ScreenHeight, 0x00000100);    // GL_DEPTH_BUFFER_BIT
                    rlDisableFramebuffer();

                    // Since our shader is now done and disabled, we can draw our m_Lights in default
                    // forward rendering
                    /*BeginMode3D(m_Camera);
                        rlEnableShader(rlGetShaderIdDefault());
                            DrawSphereEx({0.5f, 0.0f, 0.5f}, 0.2f, 8, 8, RED);
                        rlDisableShader();
                    EndMode3D();*/

                    DrawText("FINAL RESULT", 10, g_ScreenHeight - 30, 20, DARKGREEN);
                } break;
                case DEFERRED_POSITION:
                {
                    DrawTextureRec((Texture2D){
                        .id = m_GBuffer.positionTexture,
                        .width = g_ScreenWidth,
                        .height = g_ScreenHeight,
                    }, (Rectangle) { 0, 0, (float)g_ScreenWidth, (float)-g_ScreenHeight }, Vector2Zero(), RAYWHITE);

                    DrawText("POSITION TEXTURE", 10, g_ScreenHeight - 30, 20, DARKGREEN);
                } break;
                case DEFERRED_NORMAL:
                {
                    DrawTextureRec((Texture2D){
                        .id = m_GBuffer.normalTexture,
                        .width = g_ScreenWidth,
                        .height = g_ScreenHeight,
                    }, (Rectangle) { 0, 0, (float)g_ScreenWidth, (float)-g_ScreenHeight }, Vector2Zero(), RAYWHITE);

                    DrawText("NORMAL TEXTURE", 10, g_ScreenHeight - 30, 20, DARKGREEN);
                } break;
                case DEFERRED_ALBEDO:
                {
                    DrawTextureRec((Texture2D){
                        .id = m_GBuffer.albedoSpecTexture,
                        .width = g_ScreenWidth,
                        .height = g_ScreenHeight,
                    }, (Rectangle) { 0, 0, (float)g_ScreenWidth, (float)-g_ScreenHeight }, Vector2Zero(), RAYWHITE);

                    DrawText("ALBEDO TEXTURE", 10, g_ScreenHeight - 30, 20, DARKGREEN);
                } break;
                default: break;
            }

            DrawText("Switch G-buffer textures: [1][2][3][4]", 10, 70, 20, DARKGRAY);

            DrawFPS(10, 10);

        EndDrawing();
    }
}

void Application::HandleInputs()
{

    // Check key inputs to switch between G-buffer textures
    if (IsKeyPressed(KEY_ONE)) m_DeferredMode = DEFERRED_POSITION;
    if (IsKeyPressed(KEY_TWO)) m_DeferredMode = DEFERRED_NORMAL;
    if (IsKeyPressed(KEY_THREE)) m_DeferredMode = DEFERRED_ALBEDO;
    if (IsKeyPressed(KEY_FOUR)) m_DeferredMode = DEFERRED_SHADING;

    UpdateCameraPro(&m_Camera,
        (Vector3){
            (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * 0.1f -        // Move forward-backward
            (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * 0.1f,
            (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * 0.1f -     // Move right-left
            (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * 0.1f,
            IsKeyDown(KEY_SPACE) * 0.1f -
            IsKeyDown(KEY_LEFT_SHIFT) * 0.1f
        },
        (Vector3){
            GetMouseDelta().x * 0.05f,                              // Rotation: yaw
            GetMouseDelta().y * 0.05f,                              // Rotation: pitch
            0.0f                                                    // Rotation: roll
        },
        GetMouseWheelMove() * 2.0f);                                // Move to target (zoom)
}
