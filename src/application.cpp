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
#include <serializer.hpp>
#include <filedialog.hpp>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

DirectionalLight dl = {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(10.0f, 10.0f, 10.0f)};
SpotLight sl = {glm::vec3(3.0f, 1.0f, 3.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(50.0f, 0.0f, 0.0f), 30.0f, 34.0f};
PointLight pl = {glm::vec3(3.0f, 2.0f, 5.0f), glm::vec3(0.0f, 0.0f, 100.0f)};

void DrawCube(glm::vec3 cubePosition, glm::vec4 color)
{
    float cubeSize = 0.2f;

    glm::vec3 minPoint = cubePosition - glm::vec3(cubeSize / 2.0f);
    glm::vec3 maxPoint = cubePosition + glm::vec3(cubeSize / 2.0f);

    glm::vec3 p1 = glm::vec3(minPoint.x, minPoint.y, minPoint.z);
    glm::vec3 p2 = glm::vec3(maxPoint.x, minPoint.y, minPoint.z);
    glm::vec3 p3 = glm::vec3(maxPoint.x, minPoint.y, maxPoint.z);
    glm::vec3 p4 = glm::vec3(minPoint.x, minPoint.y, maxPoint.z);
    glm::vec3 p5 = glm::vec3(minPoint.x, maxPoint.y, minPoint.z);
    glm::vec3 p6 = glm::vec3(maxPoint.x, maxPoint.y, minPoint.z);
    glm::vec3 p7 = glm::vec3(maxPoint.x, maxPoint.y, maxPoint.z);
    glm::vec3 p8 = glm::vec3(minPoint.x, maxPoint.y, maxPoint.z);

    DrawLine3D(p1, p2, color);
    DrawLine3D(p2, p3, color);
    DrawLine3D(p3, p4, color);
    DrawLine3D(p4, p1, color);

    DrawLine3D(p5, p6, color);
    DrawLine3D(p6, p7, color);
    DrawLine3D(p7, p8, color);
    DrawLine3D(p8, p5, color);

    DrawLine3D(p1, p5, color);
    DrawLine3D(p2, p6, color);
    DrawLine3D(p3, p7, color);
    DrawLine3D(p4, p8, color);
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

    // Initialize G-Buffer
    m_GBuffer.Init(g_ScreenWidth, g_ScreenHeight);

    dl.shadowMap.Init();
    sl.shadowMap.Init();
    pl.shadowMap.Init();
}

void Application::Deinit()
{
    m_GBuffer.Deinit();
    dl.shadowMap.Deinit();
    sl.shadowMap.Deinit();
    pl.shadowMap.Deinit();

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

        DrawModels(GetGBufferShader(), GetCamera().GetViewMatrix());

        EnableColorBlend();

        ResetCounters();

        GetShadowMapShader().Bind();    
        dl.shadowMap.Bind();
        DrawModelsShadows(GetShadowMapShader(), dl.lightSpaceMatrix);
        dl.shadowMap.Unbind();

        sl.shadowMap.Bind();
        DrawModelsShadows(GetShadowMapShader(), sl.lightSpaceMatrix);
        sl.shadowMap.Unbind();

        GetPointLightShadowMapShader().Bind();
        GetPointLightShadowMapShader().SetUniform3fv("lightPos", pl.pos);

        for(int i = 0; i < 6; i++){
            pl.shadowMap.Bind(i);
            DrawModelsShadows(GetPointLightShadowMapShader(), pl.lightSpaceMatrix[i]);
        }

        pl.shadowMap.Unbind();

        GetDeferredShader().Bind();
        SetDirectionalLight(dl);
        SetSpotLight(sl);
        SetPointLight(pl);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, dl.shadowMap.GetShadowMap());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, sl.shadowMap.GetShadowMap());
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pl.shadowMap.GetShadowMap());

        DeferredPass(m_GBuffer, GetDeferredShader(), GetCamera(), m_DeferredMode);

        DrawFPS(1.0f / deltaTime, 10, 10);
        DrawFrameTime(deltaTime, 10, 40);
        DrawText(FormatText("Camera pos: %f %f %f", GetCamera().GetPosition().x, GetCamera().GetPosition().y, GetCamera().GetPosition().z), 10, 70, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        #ifdef DEBUG
            DrawText(FormatText("Drawn: %u Culled: %u", drawn, culled), 10, 100, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        #endif
        
        if(g_DrawBoundingBoxes) DrawBoundingBoxes();

        /*for(int i = 0; i < 10; i++){
            for(int j = 0; j < 10; j++){
                DrawLine3D(glm::vec3(i, 0, j), glm::vec3(i + 1, 0, j), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                DrawLine3D(glm::vec3(i, 0, j), glm::vec3(i, 0, j + 1), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                DrawLine3D(glm::vec3(i + 1, 0, j), glm::vec3(i + 1, 0, j + 1), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                DrawLine3D(glm::vec3(i, 0, j + 1), glm::vec3(i + 1, 0, j + 1), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }*/

        DrawCube(pl.pos, glm::vec4(pl.color, 1.0f));
        DrawCube(sl.pos, glm::vec4(sl.color, 1.0f));

        if(m_MapEditMode){
            EditMode();
        }

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

    if(!IsCursorEnabled()){
        GetCamera().ProcessMouseMovement(GetMouseXDelta(), GetMouseYDelta());
    }else{
        ResetLastMousePosition();
    }

    // Check key inputs to switch between G-buffer textures
    if(IsKeyPressed(KEY_1) && IsKeyDown(KEY_LEFT_CONTROL)) m_DeferredMode = DEFERRED_POSITION;
    if(IsKeyPressed(KEY_2) && IsKeyDown(KEY_LEFT_CONTROL)) m_DeferredMode = DEFERRED_NORMAL;
    if(IsKeyPressed(KEY_3) && IsKeyDown(KEY_LEFT_CONTROL)) m_DeferredMode = DEFERRED_ALBEDO;
    if(IsKeyPressed(KEY_4) && IsKeyDown(KEY_LEFT_CONTROL)) m_DeferredMode = DEFERRED_SHADING;

    if(!ImGui::GetIO().WantCaptureKeyboard || !m_MapEditMode){
        if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))   GetCamera().ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
        if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) GetCamera().ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) GetCamera().ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))GetCamera().ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
        if(IsKeyDown(KEY_SPACE))                    GetCamera().ProcessKeyboard(Camera::Movement::UP, deltaTime);
        if(IsKeyDown(KEY_LEFT_SHIFT))               GetCamera().ProcessKeyboard(Camera::Movement::DOWN, deltaTime);
    }

    //GetCamera().ProcessMouseScroll(GetScrollYDelta());

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
    if(IsKeyPressed(KEY_F12)) m_ShouldTakeScreenshot = true;
    if(IsKeyPressed(KEY_ESCAPE)) SetWindowShouldClose();

    UpdateProj(GetCamera().GetProjectionMatrix());
    UpdateView(GetCamera().GetViewMatrix());
}

void Application::DrawBoundingBoxes()
{
    DisableDepthTest();

    auto& models = GetModels();

    for(auto& [id, model] : models){
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

bool MouseInAABB(const glm::vec2& mouse, const glm::vec2& bb_min, const glm::vec2& bb_max)
{
    return (mouse.x >= bb_min.x && mouse.x <= bb_max.x && mouse.y >= bb_min.y && mouse.y <= bb_max.y);
}



void Application::EditMode()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse){
        std::vector<SelectedData> hovered_meshes;
        auto& models = GetModels();

        for(auto& [key, model] : models){
            for(int j = 0; j < model.GetTransforms().size(); j++){
                auto& transform = model.GetTransforms()[j];
                for(int i = 0; i < model.GetMeshes().size(); i++){
                    auto& mesh = model.GetMeshes()[i];

                    glm::vec3 bb_min = mesh.GetAABB().min;
                    glm::vec3 bb_max = mesh.GetAABB().max;

                    // transform bounding box corners to screen space
                    std::vector<glm::vec3> corners;
                    corners.push_back(glm::vec3(bb_min.x, bb_min.y, bb_min.z));
                    corners.push_back(glm::vec3(bb_max.x, bb_min.y, bb_min.z));
                    corners.push_back(glm::vec3(bb_max.x, bb_max.y, bb_min.z));
                    corners.push_back(glm::vec3(bb_min.x, bb_max.y, bb_min.z));
                    corners.push_back(glm::vec3(bb_min.x, bb_min.y, bb_max.z));
                    corners.push_back(glm::vec3(bb_max.x, bb_min.y, bb_max.z));
                    corners.push_back(glm::vec3(bb_max.x, bb_max.y, bb_max.z));
                    corners.push_back(glm::vec3(bb_min.x, bb_max.y, bb_max.z));

                    for(uint32_t i = 0; i < corners.size(); i++){
                        glm::vec4 tmp = GetCamera().GetProjectionMatrix() * GetCamera().GetViewMatrix() * transform * glm::vec4(corners[i], 1.0f);
                        corners[i] = glm::vec3(tmp) / tmp.w;
                    }

                    // create a bounding box in screen space (not really accurate for selection, maybe i will improve it later)
                    glm::vec2 min_screen = glm::vec2(corners[0].x, corners[0].y);
                    glm::vec2 max_screen = glm::vec2(corners[0].x, corners[0].y);

                    for(uint32_t i = 1; i < corners.size(); i++){
                        min_screen.x = glm::min(min_screen.x, corners[i].x);
                        min_screen.y = glm::min(min_screen.y, corners[i].y);
                        max_screen.x = glm::max(max_screen.x, corners[i].x);
                        max_screen.y = glm::max(max_screen.y, corners[i].y);
                    }

                    glm::vec2 mouse = glm::vec2(GetMousePosition().x, GetMousePosition().y);
                    mouse = glm::vec2((mouse.x / g_ScreenWidth) * 2.0f - 1.0f, (mouse.y / g_ScreenHeight) * 2.0f - 1.0f);

                    // check if mouse is inside the bounding box
                    if(MouseInAABB(mouse, min_screen, max_screen)){
                        hovered_meshes.push_back({key, i, j});
                    }
                }
            }
        }

        // sort meshes by distance to camera
        if(hovered_meshes.size() > 1){
            std::sort(hovered_meshes.begin(), hovered_meshes.end(), [&](const SelectedData& a, const SelectedData& b){
                auto& model_a = GetModel(a.model_id);
                auto& model_b = GetModel(b.model_id);

                auto& mesh_a = model_a.GetMeshes()[a.mesh_index];
                auto& mesh_b = model_b.GetMeshes()[b.mesh_index];

                AABB bb_a = mesh_a.GetAABB();
                AABB bb_b = mesh_b.GetAABB();

                bb_a.max = GetCamera().GetViewMatrix() * model_a.GetTransform(a.transform_index) * glm::vec4(bb_a.max, 1.0f);
                bb_a.min = GetCamera().GetViewMatrix() * model_a.GetTransform(a.transform_index) * glm::vec4(bb_a.min, 1.0f);
                bb_b.max = GetCamera().GetViewMatrix() * model_b.GetTransform(b.transform_index) * glm::vec4(bb_b.max, 1.0f);
                bb_b.min = GetCamera().GetViewMatrix() * model_b.GetTransform(b.transform_index) * glm::vec4(bb_b.min, 1.0f);

                glm::vec3 center_a = (bb_a.max + bb_a.min) * 0.5f;
                glm::vec3 center_b = (bb_b.max + bb_b.min) * 0.5f;

                float dist_a = glm::length(center_a);
                float dist_b = glm::length(center_b);

                return dist_a < dist_b;
            });
        }

        // select the closest mesh
        if(hovered_meshes.size() > 0){
            /*AABB aabb = GetModel(hovered_meshes[0].model_id).GetMeshes()[hovered_meshes[0].mesh_index].GetAABB();

            aabb.max = GetModel(hovered_meshes[0].model_id).GetTransform(hovered_meshes[0].transform_index) * glm::vec4(aabb.max, 1.0f);
            aabb.min = GetModel(hovered_meshes[0].model_id).GetTransform(hovered_meshes[0].transform_index) * glm::vec4(aabb.min, 1.0f);

            DrawAABB(aabb, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));*/

            m_SelectedModel = hovered_meshes[0];
        }
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

    if(ImGui::Button("Delete Selected")){
        GetModel(m_SelectedModel.model_id).RemoveTransform(m_SelectedModel.transform_index);
        UnloadModelsWithoutTransforms();
        m_SelectedModel.model_id = std::numeric_limits<uint32_t>::max();
    }

    if(ImGui::Button("Add Cube")){
        GetModel(g_Cube).AddTransform(glm::mat4(1.0f));
    }

    if(ImGui::Button("Add Sphere")){
        GetModel(g_Sphere).AddTransform(glm::mat4(1.0f));
    }

    if(ImGui::Button("Load Model")){
        ImGui::OpenPopup("Load Model");   
    }

    if(ImGui::BeginPopup("Load Model")){
        static char model_path[256] = {0};

        ImGui::InputText("Path", model_path, 256);
        ImGui::SameLine();
        if(ImGui::Button("Open")){
            char* path = OpenFile("gltf;obj;");
            if(path){
                strcpy(model_path, path);
            }
            free(path);
        }

        static bool compressed = false, pbr = false;
        ImGui::Checkbox("Compressed", &compressed);
        ImGui::Checkbox("PBR", &pbr);

        if(ImGui::Button("Load")){
            uint32_t id = LoadModel(model_path, compressed, pbr);
            GetModel(id).AddTransform(glm::mat4(1.0f));
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
        ImGuizmo::Manipulate(glm::value_ptr(GetCamera().GetViewMatrix()), glm::value_ptr(GetCamera().GetProjectionMatrix()), m_GizmoMode, ImGuizmo::MODE::WORLD, glm::value_ptr(GetModel(m_SelectedModel.model_id).GetTransform(m_SelectedModel.transform_index)));
        
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(GetModel(m_SelectedModel.model_id).GetTransform(m_SelectedModel.transform_index)), translation, rotation, scale);

        ImGui::Text("Translation");
        ImGui::SameLine();
        ImGui::InputFloat3("##translation", translation);

        ImGui::Text("Rotation");
        ImGui::SameLine();
        ImGui::InputFloat3("##rotation", rotation);

        ImGui::Text("Scale");
        ImGui::SameLine();
        ImGui::InputFloat3("##scale", scale);

        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(GetModel(m_SelectedModel.model_id).GetTransform(m_SelectedModel.transform_index)));
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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