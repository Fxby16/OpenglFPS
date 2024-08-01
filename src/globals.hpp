#pragma once

inline constexpr int g_ScreenWidth = 1920;
inline constexpr int g_ScreenHeight = 1080;
inline constexpr char* g_WindowTitle = "RaylibFPS";

#ifdef DEBUG
    extern unsigned int drawn;
    extern unsigned int culled;
#endif