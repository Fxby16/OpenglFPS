#pragma once

#include <Mesh.hpp>

constexpr float PI = 3.14159265359f;
constexpr int SPHERE_SEGMENTS = 32; // defines the smoothness of the sphere

extern Mesh CUBE_MESH;
extern Mesh SPHERE_MESH;

extern void InitPredefinedMeshes();
extern void DeinitPredefinedMeshes();