#include "raylib.h"
#include <rlgl.h>
#include <raymath.h>

#include <application.hpp>

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
    InitWindow(g_ScreenWidth, g_ScreenHeight, g_WindowTitle);

    DisableCursor();

    m_Camera.position = (Vector3){ 5.0f, 4.0f, 5.0f };    // Camera position
    m_Camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };      // Camera looking at point
    m_Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_Camera.fovy = 60.0f;                                // Camera field-of-view Y
    m_Camera.projection = CAMERA_PERSPECTIVE;

    m_Model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
    m_Cube = LoadModelFromMesh(GenMeshCube(2.0f, 2.0f, 2.0f));

    m_GBufferShader = LoadShader("resources/shaders/glsl330/gbuffer.vs",
                               "resources/shaders/glsl330/gbuffer.fs");

    m_DeferredShader = LoadShader("resources/shaders/glsl330/deferred_shading.vs",
                               "resources/shaders/glsl330/deferred_shading.fs");
    m_DeferredShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(m_DeferredShader, "viewPosition");

    m_GBuffer.framebuffer = rlLoadFramebuffer();

    if(!m_GBuffer.framebuffer){
        TraceLog(LOG_WARNING, "Failed to create framebuffer");
    }

    rlEnableFramebuffer(m_GBuffer.framebuffer);

    // Since we are storing position and normal data in these textures,
    // we need to use a floating point format.
    m_GBuffer.positionTexture = rlLoadTexture(nullptr, g_ScreenWidth, g_ScreenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);

    m_GBuffer.normalTexture = rlLoadTexture(nullptr, g_ScreenWidth, g_ScreenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32, 1);
    // color in RGB, specular strength in A.
    m_GBuffer.albedoSpecTexture = rlLoadTexture(nullptr, g_ScreenWidth, g_ScreenHeight, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

    rlActiveDrawBuffers(3);

    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.positionTexture, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.normalTexture, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.albedoSpecTexture, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);

    m_GBuffer.depthRenderbuffer = rlLoadTextureDepth(g_ScreenWidth, g_ScreenHeight, true);
    rlFramebufferAttach(m_GBuffer.framebuffer, m_GBuffer.depthRenderbuffer, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

    if (!rlFramebufferComplete(m_GBuffer.framebuffer)){
        TraceLog(LOG_WARNING, "Framebuffer is not complete");
    }

    rlEnableShader(m_DeferredShader.id);
        rlSetUniformSampler(rlGetLocationUniform(m_DeferredShader.id, "gPosition"), 0);
        rlSetUniformSampler(rlGetLocationUniform(m_DeferredShader.id, "gNormal"), 1);
        rlSetUniformSampler(rlGetLocationUniform(m_DeferredShader.id, "gAlbedoSpec"), 2);
    rlDisableShader();

    m_Model.materials[0].shader = m_GBufferShader;
    m_Cube.materials[0].shader = m_GBufferShader;

    m_Lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, Vector3Zero(), YELLOW, m_DeferredShader);
    m_Lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 2 }, Vector3Zero(), RED, m_DeferredShader);
    m_Lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, 2 }, Vector3Zero(), GREEN, m_DeferredShader);
    m_Lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, -2 }, Vector3Zero(), BLUE, m_DeferredShader);

    for (int i = 0; i < MAX_CUBES; i++){
        m_CubePositions[i] = (Vector3){
            .x = (float)(rand()%10) - 5,
            .y = (float)(rand()%5),
            .z = (float)(rand()%10) - 5,
        };

        m_CubeRotations[i] = (float)(rand()%360);
    }

    rlEnableDepthTest();

    SetTargetFPS(60);
}

void Application::Deinit()
{
    UnloadModel(m_Model);
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
    while (!WindowShouldClose())
    {
        // Update the shader with the m_Camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float m_CameraPos[3] = { m_Camera.position.x, m_Camera.position.y, m_Camera.position.z };
        SetShaderValue(m_DeferredShader, m_DeferredShader.locs[SHADER_LOC_VECTOR_VIEW], m_CameraPos, SHADER_UNIFORM_VEC3);

        HandleInputs();

        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++){
            UpdateLightValues(m_DeferredShader, m_Lights[i]);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            rlEnableFramebuffer(m_GBuffer.framebuffer);
            rlClearScreenBuffers();

            rlDisableColorBlend();
            BeginMode3D(m_Camera);

                rlEnableShader(m_GBufferShader.id);

                    DrawModel(m_Model, Vector3Zero(), 1.0f, WHITE);
                    DrawModel(m_Cube, (Vector3) { 0.0, 1.0f, 0.0 }, 1.0f, WHITE);

                    for (int i = 0; i < MAX_CUBES; i++){
                        Vector3 position = m_CubePositions[i];
                        DrawModelEx(m_Cube, position, (Vector3) { 1, 1, 1 }, m_CubeRotations[i],
                            (Vector3) { m_CubeScale, m_CubeScale, m_CubeScale }, WHITE);
                    }

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
                        rlEnableShader(m_DeferredShader.id);

                            rlActiveTextureSlot(0);
                            rlEnableTexture(m_GBuffer.positionTexture);
                            rlActiveTextureSlot(1);
                            rlEnableTexture(m_GBuffer.normalTexture);
                            rlActiveTextureSlot(2);
                            rlEnableTexture(m_GBuffer.albedoSpecTexture);

                            rlLoadDrawQuad();
                        rlDisableShader();
                        rlEnableColorBlend();
                    EndMode3D();

                    // As a last step, we now copy over the depth buffer from our g-buffer to the default framebuffer.
                    rlBindFramebuffer(RL_READ_FRAMEBUFFER, m_GBuffer.framebuffer);
                    rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, 0);
                    rlBlitFramebuffer(0, 0, g_ScreenWidth, g_ScreenHeight, 0, 0, g_ScreenWidth, g_ScreenHeight, 0x00000100);    // GL_DEPTH_BUFFER_BIT
                    rlDisableFramebuffer();

                    // Since our shader is now done and disabled, we can draw our m_Lights in default
                    // forward rendering
                    BeginMode3D(m_Camera);
                        rlEnableShader(rlGetShaderIdDefault());
                            for(int i = 0; i < MAX_LIGHTS; i++)
                            {
                                if (m_Lights[i].enabled) DrawSphereEx(m_Lights[i].position, 0.2f, 8, 8, m_Lights[i].color);
                                else DrawSphereWires(m_Lights[i].position, 0.2f, 8, 8, ColorAlpha(m_Lights[i].color, 0.3f));
                            }
                        rlDisableShader();
                    EndMode3D();

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

            DrawText("Toggle m_Lights keys: [Y][R][G][B]", 10, 40, 20, DARKGRAY);
            DrawText("Switch G-buffer textures: [1][2][3][4]", 10, 70, 20, DARKGRAY);

            DrawFPS(10, 10);

        EndDrawing();
    }
}

void Application::HandleInputs()
{
    // Check key inputs to enable/disable m_Lights
    if (IsKeyPressed(KEY_Y)) { m_Lights[0].enabled = !m_Lights[0].enabled; }
    if (IsKeyPressed(KEY_R)) { m_Lights[1].enabled = !m_Lights[1].enabled; }
    if (IsKeyPressed(KEY_G)) { m_Lights[2].enabled = !m_Lights[2].enabled; }
    if (IsKeyPressed(KEY_B)) { m_Lights[3].enabled = !m_Lights[3].enabled; }

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
