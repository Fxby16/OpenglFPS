#pragma once

#include <raylib.h>

#ifdef DEBUG
    extern unsigned int drawn;
    extern unsigned int culled;
#endif

enum FrustumPlanes{
    BACK = 0,
    FRONT = 1,
    BOTTOM = 2,
    TOP = 3,
    RIGHT = 4,
    LEFT = 5,
    NUM_PLANES = 6
};

struct Frustum{
    Vector4 Planes[NUM_PLANES];
};

extern Frustum g_Frustum;

extern void ExtractFrustum(Frustum& frustrum, Matrix projection, Matrix modelview);
extern bool PointInFrustumV(Frustum& frustrum, Vector3 position);
extern bool SphereInFrustumV(Frustum& frustrum, Vector3 position, float radius);

extern bool AABBInFrustum(Frustum& frustrum, Vector3 min, Vector3 max);