#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include <application.hpp>
#include <pbr.hpp>

#include <cstdio>
#include <string>

#include <opengl.hpp>
#include <utils.hpp>
#include <globals.hpp>
#include <frustum.hpp>

#include <GLFW/glfw3.h>

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
    //SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitWindow(g_ScreenWidth, g_ScreenHeight, g_WindowTitle);

    EnableVSync();
    DisableCursor();

    #ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLErrorCallback, 0);
    #endif

    // Camera initialization
    m_Camera.position = (Vector3){ 0.5f, 0.0f, 0.0f };    // Camera position
    m_Camera.target = (Vector3){ 1.0f, 0.0f, 0.0f };      // Camera looking at point
    m_Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_Camera.fovy = 60.0f;                                // Camera field-of-view Y
    m_Camera.projection = CAMERA_PERSPECTIVE;

    m_ProjectionMatrix = MatrixPerspective(m_Camera.fovy * DEG2RAD, (float) g_ScreenWidth / (float) g_ScreenHeight, 0.1, 1000.0);

    // Load resources
    m_rk62.Load("resources/models/rk62/scene.gltf");

    m_Map.Load("/home/fabio/the_bathroom/scene.gltf");

    m_GBufferShader.Load("resources/shaders/gbuffer.vs",
                               "resources/shaders/gbuffer.fs");

    m_DeferredShader.Load("resources/shaders/deferred_shading.vs",
                               "resources/shaders/deferred_shading.fs");
    
    // Initialize G-Buffer
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

    // Set samplers for deferred rendering shader
    m_DeferredShader.Bind();
    m_DeferredShader.SetUniform1i("Positions", 0);
    m_DeferredShader.SetUniform1i("Normals", 1);
    m_DeferredShader.SetUniform1i("Albedo", 2);

    m_GBufferShader.Bind();
    m_GBufferShader.SetUniformMat4fv("projection", m_ProjectionMatrix, 1);

    rlEnableDepthTest();
}

void Application::Deinit()
{
    m_rk62.Unload();
    m_Map.Unload();

    m_DeferredShader.Unload();
    m_GBufferShader.Unload();

    rlUnloadFramebuffer(m_GBuffer.framebuffer);
    rlUnloadTexture(m_GBuffer.positionTexture);
    rlUnloadTexture(m_GBuffer.normalTexture);
    rlUnloadTexture(m_GBuffer.albedoSpecTexture);
    rlUnloadTexture(m_GBuffer.depthRenderbuffer);

    CloseWindow();
}

void Application::Run()
{
    m_DeferredShader.Bind();
    m_DeferredShader.SetUniform1i("numLights", 1);
    m_DeferredShader.SetUniform3fv("lightColor[0]", (Vector3){5.0f, 5.0f, 5.0f});

    double lastFrameTime = GetTime();
    double deltaTime = 0.0;

    while (!WindowShouldClose())
    {
        double currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        ExtractFrustum(g_Frustum, m_ProjectionMatrix, GetCameraMatrix(m_Camera));
        #ifdef DEBUG
            drawn = 0;
            culled = 0; 
        #endif

        m_DeferredShader.Bind();
        m_DeferredShader.SetUniform3fv("camPos", m_Camera.position);
        m_DeferredShader.SetUniform3fv("lightPos[0]", m_Camera.position);

        HandleInputs(deltaTime);

        BeginDrawing();
            ClearBackground(BLACK);

            rlEnableFramebuffer(m_GBuffer.framebuffer);
            rlClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            rlClearScreenBuffers();

            rlDisableColorBlend();
            BeginMode3D(m_Camera);

                m_GBufferShader.Bind();
                Matrix model = MatrixIdentity();
                model = MatrixMultiply(MatrixTranslate(0.0f, 0.0f, 0.0f), MatrixScale(0.05f, 0.05f, 0.05f));
                m_rk62.Draw(m_GBufferShader, GetCameraMatrix(m_Camera), model);
                
                model = MatrixMultiply(MatrixTranslate(1.0f, 0.0f, 0.0f), MatrixScale(0.5f, 0.5f, 0.5f));
                m_Map.Draw(m_GBufferShader, GetCameraMatrix(m_Camera), model);    
            EndMode3D();

            rlEnableColorBlend();

            rlDisableFramebuffer();
            rlClearScreenBuffers();

            DeferredPass(m_GBuffer, m_DeferredShader.GetShader(), m_Camera, m_DeferredMode);

            DrawFPS(10, 10);
            DrawFrameTime(deltaTime, 10, 30);
            DrawText(TextFormat("Camera pos: %f %f %f", m_Camera.position.x, m_Camera.position.y, m_Camera.position.z), 10, 50, 20, WHITE);
            
            #ifdef DEBUG
                DrawText(TextFormat("Drawn: %u Culled: %u", drawn, culled), 10, 70, 20, WHITE);
            #endif

        EndDrawing();
    }
}

void Application::HandleInputs(double deltaTime)
{

    // Check key inputs to switch between G-buffer textures
    if (IsKeyPressed(KEY_ONE)) m_DeferredMode = DEFERRED_POSITION;
    if (IsKeyPressed(KEY_TWO)) m_DeferredMode = DEFERRED_NORMAL;
    if (IsKeyPressed(KEY_THREE)) m_DeferredMode = DEFERRED_ALBEDO;
    if (IsKeyPressed(KEY_FOUR)) m_DeferredMode = DEFERRED_SHADING;

    UpdateCameraPro(&m_Camera,
        (Vector3){
            (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * deltaTime  -          // Move forward-backward
            (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * deltaTime,
            (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * deltaTime -        // Move right-left
            (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * deltaTime,
            IsKeyDown(KEY_SPACE) * deltaTime -                              // Move up-down
            IsKeyDown(KEY_LEFT_SHIFT) * deltaTime
        },
        (Vector3){
            GetMouseDelta().x * deltaTime * 10,                             // Rotation: yaw
            GetMouseDelta().y * deltaTime * 10,                             // Rotation: pitch
            0.0f                                                            // Rotation: roll
        },
        GetMouseWheelMove() * 2.0f);                                        // Move to target (zoom)

    if (IsKeyPressed(KEY_F10)) ToggleVSync();
}
