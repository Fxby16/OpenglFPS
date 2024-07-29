#include <raylib.h>
#include <utils.hpp>

#include <GLFW/glfw3.h>

static bool g_VSync = false;

void DrawFrameTime(double value, int posX, int posY)
{
    Color color = LIME;                                     // Good FPS

    if(value > 33.3f && value < 66.6f) color = ORANGE;      // Warning FPS
    else if(value > 66.6f) color = RED;                     // Critical FPS

    DrawText(TextFormat("%.2f ms", value * 1000), posX, posY, 20, color);
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