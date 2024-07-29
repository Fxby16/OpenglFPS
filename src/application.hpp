#pragma once

#include <raylib.h>
#include <rlights.h>
#include <renderer.hpp>

constexpr int g_ScreenWidth = 1600;
constexpr int g_ScreenHeight = 900;
constexpr char* g_WindowTitle = "RaylibFPS";

constexpr int MAX_CUBES = 30;

class Application{
public:
    Application();
    ~Application();

    void Run();
private:
    void Init();
    void Deinit();

    void HandleInputs();

    Camera m_Camera;
    Model m_Model;
    Model m_Cube;
    Model m_rk62;
    Shader m_GBufferShader;
    Shader m_DeferredShader;
    GBuffer m_GBuffer;
    DeferredMode m_DeferredMode = DEFERRED_SHADING;
    const float m_CubeScale = 0.25;
    Vector3 m_CubePositions[MAX_CUBES];
    float m_CubeRotations[MAX_CUBES];
};
