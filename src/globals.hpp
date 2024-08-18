#pragma once


inline constexpr int g_ScreenWidth = 1600;
inline constexpr int g_ScreenHeight = 900;
inline constexpr float g_AspectRatio = static_cast<float>(g_ScreenWidth) / static_cast<float>(g_ScreenHeight);
inline constexpr float g_FOV = 60.0f;
inline constexpr float g_Near = 0.1f;
inline constexpr float g_Far = 40.0f;
inline constexpr char* g_WindowTitle = "OpenglFPS";

extern bool g_DrawBoundingBoxes;

#ifdef DEBUG
    extern unsigned int drawn;
    extern unsigned int culled;
#endif