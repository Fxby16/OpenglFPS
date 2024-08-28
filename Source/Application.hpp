#pragma once

#include <Renderer.hpp>
#include <Model.hpp>
#include <GBuffer.hpp>

#include <limits>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

class Application{
public:
    Application();
    ~Application();

    void Run();
private:
    void Init();
    void Deinit();

    //void LoadResources();

    void HandleInputs(double deltaTime);
    void DrawBoundingBoxes();
    void EditMode();

    struct SelectedData{
        uint32_t model_id;
        int mesh_index;
        int transform_index;
    };

    SelectedData m_SelectedModel;
    glm::vec2 m_EditModeLastMousePosition;
    ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;

    GBuffer m_GBuffer;
    DeferredMode m_DeferredMode = DEFERRED_SHADING;

    bool m_ShouldTakeScreenshot = false;
    bool m_MapEditMode = false;
};
