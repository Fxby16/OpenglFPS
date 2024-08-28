#pragma once

#include <glm.hpp>
#include <Camera.hpp>

#ifdef DEBUG
    extern unsigned int drawn;
    extern unsigned int culled;
#endif

enum FrustumPlanes{
    NEAR = 0,
    FAR = 1,
    BOTTOM = 2,
    TOP = 3,
    RIGHT = 4,
    LEFT = 5,
    NUM_PLANES = 6
};

struct Plane{
    glm::vec3 normal;
    float distance;
};

struct Frustum{
    Plane Planes[NUM_PLANES];
};

extern Frustum g_Frustum;

extern void DrawFrustum(Frustum& frustum);

extern void ExtractFrustum(Frustum& frustrum, const Camera& camera);
extern bool PointInFrustum(Frustum& frustrum, glm::vec3 position);
extern bool SphereInFrustum(Frustum& frustrum, glm::vec3 position, float radius);
extern bool AABBInFrustum(Frustum& frustrum, glm::vec3 min, glm::vec3 max);
extern bool OBBInFrustum(Frustum& frustrum, glm::vec3 center, glm::vec3 extents, glm::mat3 rotation);