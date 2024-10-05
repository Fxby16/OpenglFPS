bool g_DrawBoundingBoxes = false;

#ifdef DEBUG
    unsigned int drawn = 0;
    unsigned int culled = 0;
#endif

int g_ScreenWidth = 1280;
int g_ScreenHeight = 720;
float g_AspectRatio = (float)g_ScreenWidth / (float)g_ScreenHeight;