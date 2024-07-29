#pragma once

#include <raylib.h>

extern void DrawModelPBR(Model model, Vector3 position, float scale, Color tint);
extern void DrawModelPBREx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);
extern void DrawMeshPBR(Mesh mesh, Material material, Matrix transform);