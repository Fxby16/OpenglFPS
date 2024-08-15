#include <cstdio>
#include <string>
#include <thread>
#include <deque>
#include <mutex>

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
#include <resource_manager.hpp>
#include <predefined_meshes.hpp>

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

    GetCamera().Init({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, g_FOV);

    m_TexturedCube = LoadModel({CUBE_MESH}, false);
    m_TexturedSphere = LoadModel({SPHERE_MESH}, false);

    Material mat;
    mat.Load("resources/materials/lined_cement/lined_cement.mat");
    std::vector<Mesh>& cube_meshes = GetModel(m_TexturedCube).GetMeshes();

    for(auto& mesh : cube_meshes){
        mesh.ApplyMaterial(mat);
    }

    std::vector<Mesh>& sphere_meshes = GetModel(m_TexturedSphere).GetMeshes();

    for(auto& mesh : sphere_meshes){
        mesh.ApplyMaterial(mat);
    }
    
    // Initialize G-Buffer
    m_GBuffer.Init(g_ScreenWidth, g_ScreenHeight);
}

void Application::Deinit()
{
    m_GBuffer.Deinit();

    CloseWindow();
}

void Application::Run()
{
    double lastFrameTime = GetTime();
    double deltaTime = 0.0;

    while(!WindowShouldClose())
    {
        double currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        PollEvents();
        HandleInputs(deltaTime);
        
        ExtractFrustum(g_Frustum, GetCamera());

        //glm::mat4 externalCamera = glm::lookAt(glm::vec3(-3.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //UpdateView(externalCamera);

        #ifdef DEBUG
            drawn = 0;
            culled = 0; 
        #endif

        GetDeferredShader().Bind();
        GetDeferredShader().SetUniform3fv("camPos", GetCamera().GetPosition(), 1);

        ClearColor(0, 0, 0, 1);
        ClearScreen();

        m_GBuffer.Bind();
        ClearScreen();

        DisableColorBlend();

        GetGBufferShader().Bind();

        GetModel(m_TexturedCube).Draw(GetGBufferShader(), GetCamera().GetViewMatrix(), glm::mat4(1.0f));

        glm::mat4 model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));

        GetModel(m_TexturedSphere).Draw(GetGBufferShader(), GetCamera().GetViewMatrix(), model);

        EnableColorBlend();

        ResetCounters();

        PointLight pl;
        pl.pos = GetCamera().GetPosition();
        pl.color = glm::vec3(1.0f, 1.0f, 1.0f);

        SetPointLight(pl);

        /*SpotLight sl;
        sl.pos = GetCamera().GetPosition();
        sl.dir = GetCamera().GetFront();
        sl.color = glm::vec3(1.0f, 1.0f, 1.0f);
        sl.cutOff = glm::cos(glm::radians(12.5f));
        sl.outerCutOff = glm::cos(glm::radians(17.5f));

        SetSpotLight(sl);*/

        /*DirectionalLight dl;
        dl.dir = glm::vec3(0.0f, -1.0f, 0.0f);
        dl.color = glm::vec3(10.0f, 10.0f, 10.0f);

        SetDirectionalLight(dl);*/

        DeferredPass(m_GBuffer, GetDeferredShader(), GetCamera(), m_DeferredMode);

        DrawFPS(1.0f / deltaTime, 10, 10);
        DrawFrameTime(deltaTime, 10, 40);
        DrawText(FormatText("Camera pos: %f %f %f", GetCamera().GetPosition().x, GetCamera().GetPosition().y, GetCamera().GetPosition().z), 10, 70, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        #ifdef DEBUG
            DrawText(FormatText("Drawn: %u Culled: %u", drawn, culled), 10, 100, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        #endif
        
        if(g_DrawBoundingBoxes) DrawBoundingBoxes();

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

    if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))   GetCamera().ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
    if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) GetCamera().ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) GetCamera().ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))GetCamera().ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
    if(IsKeyDown(KEY_SPACE))                    GetCamera().ProcessKeyboard(Camera::Movement::UP, deltaTime);
    if(IsKeyDown(KEY_LEFT_SHIFT))               GetCamera().ProcessKeyboard(Camera::Movement::DOWN, deltaTime);

    GetCamera().ProcessMouseMovement(GetMouseXDelta(), GetMouseYDelta());
    GetCamera().ProcessMouseScroll(GetScrollYDelta());

    if(IsKeyPressed(KEY_F9)) g_DrawBoundingBoxes = !g_DrawBoundingBoxes;
    if(IsKeyPressed(KEY_F10)) ToggleVSync();
    if(IsKeyPressed(KEY_F12)) m_ShouldTakeScreenshot = true;
    if(IsKeyPressed(KEY_ESCAPE)) SetWindowShouldClose();

    UpdateProj(GetCamera().GetProjectionMatrix());
    UpdateView(GetCamera().GetViewMatrix());
}

void Application::DrawBoundingBoxes()
{
    /*DisableDepthTest();

    auto& models = GetModels();

    for(auto& [id, model] : models){
        auto& meshes = model.GetMeshes();

        for(auto &mesh : meshes){
            BoundingBox bb = mesh.GetAABB();
            bb.max = model * glm::vec4(bb.max, 1.0f);
            bb.min = model * glm::vec4(bb.min, 1.0f);

            glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

            if(!AABBInFrustum(g_Frustum, bb.min, bb.max)){
                color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }

            //LogMessage("Min: %f %f %f Max: %f %f %f Visible: %s", bb.min.x, bb.min.y, bb.min.z, bb.max.x, bb.max.y, bb.max.z, AABBInFrustum(g_Frustum, bb.min, bb.max) ? "true" : "false");

            DrawBoundingBox(bb, color);
        }
    }

    EnableDepthTest();*/
}

// Cannot be used because i currently don't have something to call opengl functions from another thread
/*
void Application::LoadResources()
{
    std::deque<std::string> logs;
    std::mutex m;
    bool loaded = false;

    SetLogsOutput(&logs, &m);

    std::thread t1([&loaded, this](){
        m_Map = LoadModel("/home/fabio/the_bathroom/scene.gltf");
        //m_Map.Load("/home/fabio/sponza/Main.1_Sponza/NewSponza_Main_glTF_003.gltf");
    
        loaded = true;
    });

    t1.detach();

    unsigned int max_lines = g_ScreenHeight / 30;

    while(!loaded){
        PollEvents();

        ClearColor(0, 0, 0, 1);
        ClearScreen();

        m.lock();
        unsigned int num_logs = logs.size();

        if(num_logs > max_lines){
            while(num_logs > max_lines){
                logs.pop_front();
                num_logs--;
            }
        }

        for(int i = 0; i < num_logs; i++){
            DrawText(logs[i], 10, 10 + i * 30, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        m.unlock();

        SwapBuffers();
    }

    UnsetLogsOutput();
}*/