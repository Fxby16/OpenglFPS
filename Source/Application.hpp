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
        int transform_index;

        SelectedData() = default;
        SelectedData(std::pair<uint32_t, unsigned int> data) : model_id(data.first), transform_index(data.second) {}
    };

    SelectedData m_SelectedModel;
    glm::vec2 m_EditModeLastMousePosition;
    ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;

    bool m_ShouldTakeScreenshot = false;
    bool m_MapEditMode = false;
};
