#include <cstdio>
#include <string>
#include <thread>
#include <deque>
#include <mutex>

#include <Application.hpp>
#include <OpenGL.hpp>
#include <Utils.hpp>
#include <Globals.hpp>
#include <Frustum.hpp>
#include <Window.hpp>
#include <Buttons.hpp>
#include <Text.hpp>
#include <Log.hpp>
#include <Lights.hpp>
#include <ResourceManager.hpp>
#include <PredefinedMeshes.hpp>
#include <Serializer.hpp>
#include <FileDialog.hpp>
#include <Timer.hpp>
#include <ComputeShader.hpp>
#include <Bloom.hpp>
#include <PostProcessing.hpp>
#include <Animation.hpp>
#include <Animator.hpp>
#include <MousePicking.hpp>
#include <Skydome.hpp>
#include <SettingsMenu.hpp>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

void DrawCube(glm::vec3 cubePosition, glm::vec4 color)
{
    DrawSolidCube(cubePosition, glm::vec3(0.2f), color);
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
    InitWindow(g_ScreenWidth, g_ScreenHeight, g_WindowTitle);

    DisableCursor();

    GetCamera().Init({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, g_FOV);

    DeserializeMap("map.txt");

    LoadDirectionalLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
    LoadSpotLight(glm::vec3(3.0f, 1.0f, 3.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 1.0f), 30.0f, 34.0f);
    LoadPointLight(glm::vec3(3.0f, 2.0f, 5.0f), glm::vec3(1.0f, 1.0f, 10.0f));

    SetDirtTexture("Resources/DirtMasks/DirtMask.jpg");
    LoadSkydome("Resources/HDRI/kloppenheim_02_puresky_4k.hdr");

    InitSettingsMenu();
}

void Application::Deinit()
{
    UnloadSkydome();
    DeinitSettingsMenu();

    CloseWindow();
}

void Application::Run()
{
    double lastFrameTime = GetTime();
    double deltaTime = 0.0;
    double lastTime = GetTime();

    ShouldDisplayTimers(true);

    while(!WindowShouldClose()){
        double currentFrameTime = GetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if(GetTime() - lastTime >= 1.0){
            lastTime = GetTime();
            ShouldDisplayTimers(true);
        }

        PollEvents();
        HandleInputs(deltaTime);

        constexpr uint32_t saiga_id = 2398989031;
        SkinnedModel* saiga = GetSkinnedModel(saiga_id);

        if(saiga){
            for(int i = KEY_0; i <= KEY_9; i++){
                if(IsKeyPressed(i)){
                    saiga->animator.SetCurrentAnimation(i - KEY_0);
                    saiga->animator.SetLooping(false);
                    saiga->animator.PlayAnimation();
                }
            }

            if(!saiga->animator.IsPlaying()){
                saiga->animator.SetCurrentAnimation(0);
                saiga->animator.SetLooping(true);
                saiga->animator.PlayAnimation();
            }
        }

        UpdateAnimations(deltaTime);
        
        ExtractFrustum(g_Frustum, GetCamera());

        //glm::mat4 externalCamera = glm::lookAt(glm::vec3(-3.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //UpdateView(externalCamera);

        #ifdef DEBUG
            drawn = 0;
            culled = 0; 
        #endif

        Timer timer2("GBUFFER_PASS");

        GetDeferredShader().Bind();
        GetDeferredShader().SetUniform3fv("camPos", GetCamera().GetPosition(), 1);

        ClearColor(0, 0, 0, 1);
        ClearScreen();

        GBuffer& gbuffer = GetGBuffer();
        gbuffer.Bind();
        ClearScreen();

        DisableColorBlend();

        DrawModels(GetGBufferShader(), GetCamera().GetViewMatrix());

        EnableColorBlend();

        timer2.PrintTime();

        Timer timer3("SHADOW_MAPPING");

        GetDeferredShader().Bind();

        DrawShadowMaps();
        SetShadowMaps();

        timer3.PrintTime();

        Timer timer4("DEFERRED_PASS");
        DeferredPass(gbuffer, GetDeferredShader(), GetCamera());
        timer4.PrintTime();

        std::unordered_map<uint32_t, PointLight>& pointLights = GetPointLights();
        for(auto& [id, pointLight] : pointLights){
            DrawCube(pointLight.pos, glm::vec4(pointLight.color, 1.0f));
        }

        std::unordered_map<uint32_t, SpotLight>& spotLights = GetSpotLights();
        for(auto& [id, spotLight] : spotLights){
            DrawCube(spotLight.pos, glm::vec4(spotLight.color, 1.0f));
        }

        DrawSkydome(GetCamera().GetViewMatrix(), GetCamera().GetProjectionMatrix(), glm::translate(glm::mat4(1.0f), GetCamera().GetPosition()));

        //bloom pass

        Timer timer6("BLOOM_PASS");

        if(GetUseBloom()){
            BloomPass();
        }

        UnbindFramebuffer();

        timer6.PrintTime();

        PostProcessingPass();

        DrawFPS(1.0f / deltaTime, 10, 10);
        DrawFrameTime(deltaTime, 10, 40);
        DrawText(FormatText("Camera pos: %f %f %f", GetCamera().GetPosition().x, GetCamera().GetPosition().y, GetCamera().GetPosition().z), 10, 70, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        #ifdef DEBUG
            DrawText(FormatText("Drawn: %u Culled: %u", drawn, culled), 10, 100, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        #endif
        
        if(g_DrawBoundingBoxes) DrawBoundingBoxes();

        if(GetShowSettingsMenu()){
            SettingsMenu();
        }

        if(m_MapEditMode && !GetShowSettingsMenu()){
            EditMode();
        }

        SwapBuffers();

        if(m_ShouldTakeScreenshot){
            TakeScreenshot();
            m_ShouldTakeScreenshot = false;
        }

        ShouldDisplayTimers(false);
    }
}

void Application::HandleInputs(double deltaTime)
{
    UpdateInputs();

    if(!IsCursorEnabled()){
        GetCamera().ProcessMouseMovement(GetMouseXDelta(), GetMouseYDelta());
    }else{
        ResetLastMousePosition();
    }

    // Check key inputs to switch between G-buffer textures
    if(IsKeyPressed(KEY_1) && IsKeyDown(KEY_LEFT_CONTROL)) SetDeferredMode(DEFERRED_POSITION);
    if(IsKeyPressed(KEY_2) && IsKeyDown(KEY_LEFT_CONTROL)) SetDeferredMode(DEFERRED_NORMAL);
    if(IsKeyPressed(KEY_3) && IsKeyDown(KEY_LEFT_CONTROL)) SetDeferredMode(DEFERRED_ALBEDO);
    if(IsKeyPressed(KEY_4) && IsKeyDown(KEY_LEFT_CONTROL)) SetDeferredMode(DEFERRED_SHADING);

    if(!ImGui::GetIO().WantCaptureKeyboard || !m_MapEditMode){
        if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))   GetCamera().ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
        if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) GetCamera().ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) GetCamera().ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))GetCamera().ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
        if(IsKeyDown(KEY_SPACE))                    GetCamera().ProcessKeyboard(Camera::Movement::UP, deltaTime);
        if(IsKeyDown(KEY_LEFT_SHIFT))               GetCamera().ProcessKeyboard(Camera::Movement::DOWN, deltaTime);
    }

    //GetCamera().ProcessMouseScroll(GetScrollYDelta());

    if(IsKeyPressed(KEY_F5)){
        SetShowSettingsMenu(!GetShowSettingsMenu());
        if(GetShowSettingsMenu()){
            EnableCursor();
        }else{
            DisableCursor();
        }
    }
    if(IsKeyPressed(KEY_F6)){
        m_MapEditMode = !m_MapEditMode;
        m_SelectedModel.model_id = std::numeric_limits<uint32_t>::max();
        m_EditModeLastMousePosition = GetMousePosition();
        if(m_MapEditMode){
            EnableCursor();
        }else{
            DisableCursor();
        }
    }
    if(IsKeyPressed(KEY_F7)) ToggleCursor();
    if(IsKeyPressed(KEY_F8)) HotReloadShaders();
    if(IsKeyPressed(KEY_F9)) g_DrawBoundingBoxes = !g_DrawBoundingBoxes;
    if(IsKeyPressed(KEY_F10)) ToggleVSync();
    if(IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if(IsKeyPressed(KEY_F12) && IsKeyDown(KEY_LEFT_CONTROL)) m_ShouldTakeScreenshot = true;
    if(IsKeyPressed(KEY_ESCAPE)) SetWindowShouldClose();

    UpdateProj(GetCamera().GetProjectionMatrix());
    UpdateView(GetCamera().GetViewMatrix());
}

void Application::DrawBoundingBoxes()
{
    DisableDepthTest();

    auto& Models = GetModels();

    for(auto& [id, model] : Models){
        auto& transforms = model.GetTransforms();

        for(uint32_t i = 0; i < transforms.size(); i++){
            auto& meshes = model.GetMeshes();
            auto& transform = transforms[i];

            for(auto &mesh : meshes){
                AABB bb = mesh.GetAABB();
                OBB obb = OBBFromAABB(bb, transform); //use the OBB so you can rotate the model

                glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

                if(!OBBInFrustum(g_Frustum, obb.center, obb.extents, obb.rotation)){
                    color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                }

                //Get corners of the OBB. You could just draw transform * AABB but this ensures that the OBB is correct 
                std::vector<glm::vec3> corners(8);
    
                glm::vec3 axisX = obb.rotation[0];
                glm::vec3 axisY = obb.rotation[1];
                glm::vec3 axisZ = obb.rotation[2];
                glm::vec3 halfExtents = obb.extents;

                corners[0] = obb.center + axisX * halfExtents.x + axisY * halfExtents.y + axisZ * halfExtents.z;
                corners[1] = obb.center + axisX * halfExtents.x + axisY * halfExtents.y - axisZ * halfExtents.z;
                corners[2] = obb.center + axisX * halfExtents.x - axisY * halfExtents.y + axisZ * halfExtents.z;
                corners[3] = obb.center + axisX * halfExtents.x - axisY * halfExtents.y - axisZ * halfExtents.z;
                corners[4] = obb.center - axisX * halfExtents.x + axisY * halfExtents.y + axisZ * halfExtents.z;
                corners[5] = obb.center - axisX * halfExtents.x + axisY * halfExtents.y - axisZ * halfExtents.z;
                corners[6] = obb.center - axisX * halfExtents.x - axisY * halfExtents.y + axisZ * halfExtents.z;
                corners[7] = obb.center - axisX * halfExtents.x - axisY * halfExtents.y - axisZ * halfExtents.z;
            
                DrawLine3D(corners[0], corners[1], color);
                DrawLine3D(corners[1], corners[3], color);
                DrawLine3D(corners[3], corners[2], color);
                DrawLine3D(corners[2], corners[0], color);

                DrawLine3D(corners[4], corners[5], color);
                DrawLine3D(corners[5], corners[7], color);
                DrawLine3D(corners[7], corners[6], color);
                DrawLine3D(corners[6], corners[4], color);

                DrawLine3D(corners[0], corners[4], color);
                DrawLine3D(corners[1], corners[5], color);
                DrawLine3D(corners[2], corners[6], color);
                DrawLine3D(corners[3], corners[7], color);
            }
        }
    }

    EnableDepthTest();
}

void Application::EditMode()
{
    UpdateMousePicking();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse){
        m_SelectedModel = GetSelectedModel(GetMousePosition());
    }

    if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
        glm::vec2 mouse = GetMousePosition();
        glm::vec2 delta = mouse - m_EditModeLastMousePosition;
        m_EditModeLastMousePosition = mouse;
    
        GetCamera().ProcessMouseMovement(delta.x, delta.y);
    }else{
        m_EditModeLastMousePosition = GetMousePosition();
    }

    ImGui::Begin("Menu");
    ImGui::SetWindowSize(ImVec2(0, 0));

    Model* selected_model = nullptr;
    selected_model = GetModel(m_SelectedModel.model_id);
    if(!selected_model){
        SkinnedModel* skinned_model = GetSkinnedModel(m_SelectedModel.model_id);
        if(skinned_model){
            selected_model = &skinned_model->model;
        }
    }

    if(ImGui::Button("Delete Selected") && m_SelectedModel.model_id != std::numeric_limits<uint32_t>::max()){
        selected_model->RemoveTransform(m_SelectedModel.transform_index);
        UnloadModelsWithoutTransforms();
        m_SelectedModel.model_id = std::numeric_limits<uint32_t>::max();
    }

    if(ImGui::Button("Add Cube")){
        GetModel(g_Cube)->AddTransform(glm::mat4(1.0f));
    }

    if(ImGui::Button("Add Sphere")){
        GetModel(g_Sphere)->AddTransform(glm::mat4(1.0f));
    }

    if(ImGui::Button("Load Model")){
        ImGui::OpenPopup("Load Model");   
    }

    if(ImGui::Button("Load Animated Model")){
        ImGui::OpenPopup("Load Animated Model");
    }

    if(ImGui::Button("Add Animation") && GetSkinnedModel(m_SelectedModel.model_id) != nullptr){
        ImGui::OpenPopup("Add Animation");
    }

    if(ImGui::BeginPopup("Load Model")){
        static char model_path[256] = {0};

        ImGui::InputText("Path", model_path, 256);
        ImGui::SameLine();
        if(ImGui::Button("Open")){
            char* path = OpenFile("gltf;obj;dae;");
            if(path){
                strcpy(model_path, path);
            }
            free(path);
        }

        if(ImGui::Button("Load")){
            uint32_t id = LoadModel(model_path);
            GetModel(id)->AddTransform(glm::mat4(1.0f));
        }

        ImGui::EndPopup();
    }

    if(ImGui::BeginPopup("Load Animated Model")){
        static char model_path[256] = {0};
        static char animation_path[256] = {0};
        static float ticks_per_second = 0.0f;

        ImGui::InputText("Path", model_path, 256);
        ImGui::SameLine();
        if(ImGui::Button("Open")){
            char* path = OpenFile("gltf;obj;dae;");
            if(path){
                strcpy(model_path, path);
            }
            free(path);
        }

        ImGui::InputText("Animation Path", animation_path, 256);
        ImGui::SameLine();
        if(ImGui::Button("Open##2")){
            char* path = OpenFile("gltf;obj;dae;fbx;");
            if(path){
                strcpy(animation_path, path);
            }
            free(path);
        }

        ImGui::InputFloat("Ticks Per Second", &ticks_per_second);

        if(ImGui::Button("Load")){
            uint32_t id = LoadSkinnedModel(model_path, animation_path, ticks_per_second);
            GetSkinnedModel(id)->model.AddTransform(glm::mat4(1.0f));
        }

        ImGui::EndPopup();
    }

    if(ImGui::BeginPopup("Add Animation")){
        static char animation_path[256] = {0};
        static float ticks_per_second = 0.0f;

        ImGui::InputText("Animation Path", animation_path, 256);
        ImGui::SameLine();
        if(ImGui::Button("Open")){
            char* path = OpenFile("gltf;obj;fbx;dae;");
            if(path){
                strcpy(animation_path, path);
            }
            free(path);
        }

        ImGui::InputFloat("Ticks Per Second", &ticks_per_second);

        if(ImGui::Button("Add")){
            GetSkinnedModel(m_SelectedModel.model_id)->AddAnimation(animation_path, ticks_per_second);
        }

        ImGui::EndPopup();
    }

    if(ImGui::Button("Save Map")){
        SerializeMap("map.txt");
    }

    if(ImGui::Button("Load Map")){
        ClearModels();
        DeserializeMap("map.txt");
    }

    int gizmo_mode;

    switch(m_GizmoMode){
        case ImGuizmo::OPERATION::TRANSLATE:
            gizmo_mode = 0;
            break;
        case ImGuizmo::OPERATION::ROTATE:
            gizmo_mode = 1;
            break;
        case ImGuizmo::OPERATION::SCALE:
            gizmo_mode = 2;
            break;
        default:
            gizmo_mode = 0;
            break;
    }

    ImGui::Text("Gizmo Mode");
    ImGui::SameLine();
    ImGui::Combo("##gizmomode", &gizmo_mode, "Translate\0Rotate\0Scale\0");

    switch(gizmo_mode){
        case 0:
            m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case 1:
            m_GizmoMode = ImGuizmo::OPERATION::ROTATE;
            break;
        case 2:
            m_GizmoMode = ImGuizmo::OPERATION::SCALE;
            break;
        default:
            m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
            break;
    }

    float translation[3], rotation[3], scale[3];
    if(m_SelectedModel.model_id != std::numeric_limits<uint32_t>::max()){
        ImGuizmo::SetRect(0, 0, g_ScreenWidth, g_ScreenHeight);
        ImGuizmo::Manipulate(glm::value_ptr(GetCamera().GetViewMatrix()), glm::value_ptr(GetCamera().GetProjectionMatrix()), m_GizmoMode, ImGuizmo::MODE::WORLD, glm::value_ptr(selected_model->GetTransform(m_SelectedModel.transform_index)));
        
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(selected_model->GetTransform(m_SelectedModel.transform_index)), translation, rotation, scale);

        ImGui::Text("Translation");
        ImGui::SameLine();
        ImGui::InputFloat3("##translation", translation);

        ImGui::Text("Rotation");
        ImGui::SameLine();
        ImGui::InputFloat3("##rotation", rotation);

        ImGui::Text("Scale");
        ImGui::SameLine();
        ImGui::InputFloat3("##scale", scale);

        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(selected_model->GetTransform(m_SelectedModel.transform_index)));
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
