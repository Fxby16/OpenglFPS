#pragma once


extern int g_ScreenWidth;
extern int g_ScreenHeight;
extern float g_AspectRatio;
inline constexpr float g_FOV = 60.0f;
inline constexpr float g_Near = 0.1f;
inline constexpr float g_Far = 40.0f;
inline constexpr const char* g_WindowTitle = "OpenglFPS";

inline constexpr unsigned int MAX_LIGHTS = 10;

extern bool g_DrawBoundingBoxes;

#ifdef DEBUG
    extern unsigned int drawn;
    extern unsigned int culled;
#endif