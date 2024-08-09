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

    void HandleInputs(double deltaTime);

    Camera m_Camera;
    //Model m_rk62;
    Model m_Map;
    Shader m_GBufferShader;
    Shader m_DeferredShader;
    GBuffer m_GBuffer;
    DeferredMode m_DeferredMode = DEFERRED_SHADING;

    bool m_ShouldTakeScreenshot = false;
};
