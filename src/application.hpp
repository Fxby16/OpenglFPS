#pragma once

#include <raylib.h>
#include <rlights.h>
#include <renderer.hpp>
#include <model.hpp>

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
    ModelEx m_rk62;
    ModelEx m_Map;
    ShaderEx m_GBufferShader;
    ShaderEx m_DeferredShader;
    GBuffer m_GBuffer;
    DeferredMode m_DeferredMode = DEFERRED_SHADING;
    Matrix m_ProjectionMatrix;
};
