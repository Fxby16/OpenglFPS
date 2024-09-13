#include <Window.hpp>
#include <OpenGL.hpp>
#include <Log.hpp>
#include <Input.hpp>
#include <Renderer.hpp>
#include <Text.hpp>
#include <ResourceManager.hpp>
#include <Camera.hpp>
#include <PredefinedMeshes.hpp>
#include <Bloom.hpp>
#include <PostProcessing.hpp>
#include <Timer.hpp>
#include <MousePicking.hpp>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>

static bool g_VSync = false;
static double g_ScrollYOffset = 0;

static GLFWwindow* g_Window = nullptr;

static Input g_Input;

int InitWindow(unsigned int width, unsigned int height, const char* title)
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    g_Window = glfwCreateWindow(width, height, title, nullptr, nullptr); 

    if(g_Window == nullptr){
        LogError("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(g_Window);
    glfwSetFramebufferSizeCallback(g_Window, OpenglFramebufferSizeCallback);
    glfwSetScrollCallback(g_Window, GlfwScrollCallback);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        LogError("Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

    #ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLErrorCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);  
    #endif

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    DisableVSync();
    EnableDepthTest();
    SetCullFace(GL_BACK);
    SetWindingOrder(GL_CCW);
    EnableCullFace();

    // modules initialization
    GetCamera().Init({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, g_FOV);
    InitRenderer();
    InitTextRenderer("Resources/Fonts/tektur/Tektur-Regular.ttf", 30);
    InitPredefinedMeshes();
    InitResourceManager();
    InitBloom();
    InitPostProcessing();
    InitMousePicking();

    g_GBufferShader = LoadShader("Resources/Shaders/GBuffer.vert",
                                 "Resources/Shaders/GBuffer.frag");

    g_DeferredShader = LoadShader("Resources/Shaders/DeferredShading.vert",
                                  "Resources/Shaders/DeferredShading.frag");

    g_ShadowMapShader = LoadShader("Resources/Shaders/ShadowMap.vert",
                                   "Resources/Shaders/ShadowMap.frag");

    g_PointLightShadowMapShader = LoadShader("Resources/Shaders/ShadowMap.vert",
                                             "Resources/Shaders/PointLightShadowMap.frag");

    Shader& gbuffer_s = GetGBufferShader();
    gbuffer_s.Bind();
    gbuffer_s.SetUniformMat4fv("projection", GetCamera().GetProjectionMatrix(), 1);

    Shader& deferred_s = GetDeferredShader();
    deferred_s.Bind();
    deferred_s.SetUniform1i("Positions", 0);
    deferred_s.SetUniform1i("Normals", 1);
    deferred_s.SetUniform1i("Albedo", 2);
    deferred_s.SetUniform1i("ShadowMaps", 3);
    deferred_s.SetUniform1i("ShadowCubeMaps", 4);
    deferred_s.SetUniform1i("isPlaying", 0);

    Shader& shadowmap_s = GetShadowMapShader();
    shadowmap_s.Bind();
    shadowmap_s.SetUniform1i("isPlaying", 0);

    Shader& pointlightshadowmap_s = GetPointLightShadowMapShader();
    pointlightshadowmap_s.Bind();
    pointlightshadowmap_s.SetUniform1i("isPlaying", 0);

    return 0;
}

void CloseWindow()
{
    DeinitRenderer();
    DeinitTextRenderer();
    DeinitPredefinedMeshes();
    DeinitBloom();
    DeinitResourceManager();
    DeinitMousePicking();
    FreeRemainingTimers();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

GLFWwindow* GetWindow()
{
    return g_Window;
}

void ToggleVSync()
{
    g_VSync = !g_VSync;
    
    if(g_VSync) EnableVSync();
    else DisableVSync();
}

void EnableVSync()
{
    glfwSwapInterval(1);
    g_VSync = true;
}

void DisableVSync()
{
    glfwSwapInterval(0);
    g_VSync = false;
}

bool IsVSyncEnabled()
{
    return g_VSync;
}

void EnableCursor()
{
    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void DisableCursor()
{
    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool IsCursorEnabled()
{
    return glfwGetInputMode(g_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void ToggleCursor()
{
    if(IsCursorEnabled()){
        DisableCursor();
    }else{ 
        EnableCursor();
    }
}

double GetTime()
{
    return glfwGetTime();
}

bool WindowShouldClose()
{
    return glfwWindowShouldClose(g_Window);
}

void SetWindowShouldClose()
{
    glfwSetWindowShouldClose(g_Window, true);
}

void PollEvents()
{
    glfwPollEvents();
}

void SwapBuffers()
{
    glfwSwapBuffers(g_Window);
}

void GlfwScrollCallback(GLFWwindow* g_Window, double xoffset, double yoffset)
{
    g_ScrollYOffset = yoffset;
}

double GetScrollYDelta()
{
    return g_ScrollYOffset;
}

bool IsKeyPressed(int key)
{
    KeyState state = g_Input.GetKey(key);
    return state.current && !state.previous;
}

bool IsKeyReleased(int key)
{
    KeyState state = g_Input.GetKey(key);
    return !state.current && state.previous;
}

bool IsKeyDown(int key)
{
    return g_Input.GetKey(key).current;
}

bool IsKeyUp(int key)
{
    return !g_Input.GetKey(key).current;
}

bool IsMouseButtonPressed(int button)
{
    KeyState state = g_Input.GetMouseButton(button);
    return state.current && !state.previous;
}

bool IsMouseButtonReleased(int button)
{
    KeyState state = g_Input.GetMouseButton(button);
    return !state.current && state.previous;
}

bool IsMouseButtonDown(int button)
{
    return g_Input.GetMouseButton(button).current;
}

bool IsMouseButtonUp(int button)
{
    return !g_Input.GetMouseButton(button).current;
}

glm::vec2 GetMousePosition()
{
    return g_Input.GetMousePosition();
}

int GetMouseXDelta()
{
    return g_Input.GetMouseXDelta();
}

int GetMouseYDelta()
{
    return g_Input.GetMouseYDelta();
}

void UpdateInputs()
{
    g_Input.UpdateStates();
}

void ResetLastMousePosition()
{
    g_Input.ResetLastMousePosition();
}