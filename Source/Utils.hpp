#pragma once

#include <glm.hpp>
#include <assimp/matrix4x4.h>

extern void DrawFrameTime(double value, int posX, int posY);
extern void DrawFPS(double value, int posX, int posY);
extern void TakeScreenshot();
extern glm::mat4 AiToGlm(const aiMatrix4x4& from);