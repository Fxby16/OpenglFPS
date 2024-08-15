#pragma once

#include <renderer.hpp>
#include <model.hpp>
#include <gbuffer.hpp>

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

    uint32_t m_TexturedCube;
    uint32_t m_TexturedSphere;
    GBuffer m_GBuffer;
    DeferredMode m_DeferredMode = DEFERRED_SHADING;

    bool m_ShouldTakeScreenshot = false;
};
