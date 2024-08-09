#include <cstdio>
#include <string>

#include <application.hpp>
#include <opengl.hpp>
#include <utils.hpp>
#include <globals.hpp>
#include <frustum.hpp>
#include <glfw.hpp>
#include <buttons.hpp>
#include <text.hpp>
#include <log.hpp>
#include <lights.hpp>

#include <gtc/matrix_transform.hpp>

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

    m_Camera.Init({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, g_FOV);

    // Load resources
    //m_rk62.Load("resources/models/rk62/scene.gltf");

    m_Map.Load("/home/fabio/the_bathroom/scene.gltf");
    //m_Map.Load("/home/fabio/sponza/Main.1_Sponza/NewSponza_Main_glTF_003.gltf");

    m_GBufferShader.Load("resources/shaders/gbuffer.vs",
                               "resources/shaders/gbuffer.fs");

    m_DeferredShader.Load("resources/shaders/deferred_shading.vs",
                               "resources/shaders/deferred_shading.fs");
    
    // Initialize G-Buffer
    m_GBuffer.Init(g_ScreenWidth, g_ScreenHeight);

    // Set samplers for deferred rendering shader
    m_DeferredShader.Bind();
    m_DeferredShader.SetUniform1i("Positions", 0);
    m_DeferredShader.SetUniform1i("Normals", 1);
    m_DeferredShader.SetUniform1i("Albedo", 2);

    m_GBufferShader.Bind();
    m_GBufferShader.SetUniformMat4fv("projection", m_Camera.GetProjectionMatrix(), 1);

    SetLightShader(&m_DeferredShader);
}

void Application::Deinit()
{
    //m_rk62.Unload();
    m_Map.Unload();

    m_DeferredShader.Unload();
    m_GBufferShader.Unload();

    m_GBuffer.Deinit();

    CloseWindow();
}

void Application::Run()
{
    m_DeferredShader.Bind();

    double lastFrameTime = GetTime();
    double deltaTime = 0.0;

    while(!WindowShouldClose())
    {
        double currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        PollEvents();
        HandleInputs(deltaTime);
        
        ExtractFrustum(g_Frustum, m_Camera);

        //glm::mat4 externalCamera = glm::lookAt(glm::vec3(-3.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //UpdateView(externalCamera);

        #ifdef DEBUG
            drawn = 0;
            culled = 0; 
        #endif

        m_DeferredShader.Bind();
        m_DeferredShader.SetUniform3fv("camPos", m_Camera.GetPosition(), 1);

        ClearColor(0, 0, 0, 1);
        ClearScreen();

        m_GBuffer.Bind();
        ClearScreen();

        DisableColorBlend();

        m_GBufferShader.Bind();
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));

        //m_rk62.Draw(m_GBufferShader, m_Camera.GetViewMatrix(), model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        
        m_Map.Draw(m_GBufferShader, m_Camera.GetViewMatrix(), model);    
        //m_Map.Draw(m_GBufferShader, externalCamera, model);    

        EnableColorBlend();

        ResetCounters();

        SpotLight sl;
        sl.pos = m_Camera.GetPosition();
        sl.dir = m_Camera.GetFront();
        sl.color = glm::vec3(1.0f, 1.0f, 1.0f);
        sl.cutOff = glm::cos(glm::radians(12.5f));
        sl.outerCutOff = glm::cos(glm::radians(17.5f));

        SetSpotLight(sl);

        /*DirectionalLight dl;
        dl.dir = glm::vec3(0.0f, -1.0f, 0.0f);
        dl.color = glm::vec3(10.0f, 10.0f, 10.0f);

        SetDirectionalLight(dl);*/

        DeferredPass(m_GBuffer, m_DeferredShader, m_Camera, m_DeferredMode);

        DrawFPS(1.0f / deltaTime, 10, 10);
        DrawFrameTime(deltaTime, 10, 40);
        DrawText(FormatText("Camera pos: %f %f %f", m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z), 10, 70, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        #ifdef DEBUG
            DrawText(FormatText("Drawn: %u Culled: %u", drawn, culled), 10, 100, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        #endif

        /*auto meshes = m_Map.GetMeshes();

        DisableDepthTest();

        for(auto &mesh : meshes){
            BoundingBox bb = mesh.GetAABB();
            bb.max = model * glm::vec4(bb.max, 1.0f);
            bb.min = model * glm::vec4(bb.min, 1.0f);

            glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

            if(!AABBInFrustum(g_Frustum, bb.min, bb.max)){
                color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }

            LogMessage("Min: %f %f %f Max: %f %f %f Visible: %s", bb.min.x, bb.min.y, bb.min.z, bb.max.x, bb.max.y, bb.max.z, AABBInFrustum(g_Frustum, bb.min, bb.max) ? "true" : "false");

            DrawBoundingBox(bb, color);
        }

        DrawFrustum(g_Frustum);

        EnableDepthTest();*/

        SwapBuffers();

        if(m_ShouldTakeScreenshot){
            TakeScreenshot();
            m_ShouldTakeScreenshot = false;
        }
    }
}

void Application::HandleInputs(double deltaTime)
{
    UpdateInputs();

    // Check key inputs to switch between G-buffer textures
    if(IsKeyPressed(KEY_1)) m_DeferredMode = DEFERRED_POSITION;
    if(IsKeyPressed(KEY_2)) m_DeferredMode = DEFERRED_NORMAL;
    if(IsKeyPressed(KEY_3)) m_DeferredMode = DEFERRED_ALBEDO;
    if(IsKeyPressed(KEY_4)) m_DeferredMode = DEFERRED_SHADING;

    if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))   m_Camera.ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
    if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) m_Camera.ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) m_Camera.ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))m_Camera.ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
    if(IsKeyDown(KEY_SPACE))                    m_Camera.ProcessKeyboard(Camera::Movement::UP, deltaTime);
    if(IsKeyDown(KEY_LEFT_SHIFT))               m_Camera.ProcessKeyboard(Camera::Movement::DOWN, deltaTime);

    m_Camera.ProcessMouseMovement(GetMouseXDelta(), GetMouseYDelta());
    m_Camera.ProcessMouseScroll(GetScrollYDelta());

    if(IsKeyPressed(KEY_F10)) ToggleVSync();
    if(IsKeyPressed(KEY_ESCAPE)) SetWindowShouldClose();
    if(IsKeyPressed(KEY_F12)) m_ShouldTakeScreenshot = true;

    UpdateProj(m_Camera.GetProjectionMatrix());
    UpdateView(m_Camera.GetViewMatrix());
}
