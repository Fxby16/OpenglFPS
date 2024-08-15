#include <frustum.hpp>
#include <renderer.hpp>
#include <globals.hpp>
#include <log.hpp>

#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_access.hpp>

Frustum g_Frustum;
glm::vec3 g_FrustumCorners[8];

void DrawFrustum(Frustum& frustum)
{
    // Draw the far face
    DrawLine3D(g_FrustumCorners[0], g_FrustumCorners[1], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[1], g_FrustumCorners[2], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[2], g_FrustumCorners[3], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[3], g_FrustumCorners[0], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    // Draw the near face
    DrawLine3D(g_FrustumCorners[4], g_FrustumCorners[5], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[5], g_FrustumCorners[6], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[6], g_FrustumCorners[7], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[7], g_FrustumCorners[4], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    // Draw the edges connecting far and near faces
    DrawLine3D(g_FrustumCorners[0], g_FrustumCorners[4], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[1], g_FrustumCorners[5], glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[2], g_FrustumCorners[6], glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[3], g_FrustumCorners[7], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void NormalizePlane(glm::vec4& plane)
{
    float magnitude = sqrtf(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
    plane.x /= magnitude;
    plane.y /= magnitude;
    plane.z /= magnitude;
    plane.w /= magnitude;
}

Plane PlaneFromCorners(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);
    normal = glm::normalize(normal);

    Plane plane;
    plane.normal = normal;
    plane.distance = glm::dot(normal, p1);
    return plane;
}

void ExtractFrustum(Frustum& frustum, const Camera& camera)
{
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 projectionMatrix = camera.GetProjectionMatrix();

    // thanks to https://github.com/livinamuk for the corners extraction code
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
    glm::vec3 viewPos = glm::vec3(inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2]);
    glm::vec3 camForward = -glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
    glm::vec3 camRight = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    glm::vec3 camUp = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
    float fov = 2.0f * atan(1.0f / projectionMatrix[1][1]);
    float aspectRatio = projectionMatrix[1][1] / projectionMatrix[0][0];
    float nearPlane = projectionMatrix[3][2] / (projectionMatrix[2][2] - 1.0f);
    float farPlane = projectionMatrix[3][2] / (projectionMatrix[2][2] + 1.0f);
    glm::vec3 fc = viewPos + camForward * farPlane;
    glm::vec3 nc = viewPos + camForward * nearPlane;
    float Hfar = 2.0f * tan(fov / 2) * farPlane;
    float Wfar = Hfar * aspectRatio;
    float Hnear = 2.0f * tan(fov / 2) * nearPlane;
    float Wnear = Hnear * aspectRatio;
    glm::vec3 up = camUp;
    glm::vec3 right = camRight;
    glm::vec3 ftl = fc + (up * Hfar / 2.0f) - (right * Wfar / 2.0f);
    glm::vec3 ftr = fc + (up * Hfar / 2.0f) + (right * Wfar / 2.0f);
    glm::vec3 fbl = fc - (up * Hfar / 2.0f) - (right * Wfar / 2.0f);
    glm::vec3 fbr = fc - (up * Hfar / 2.0f) + (right * Wfar / 2.0f);
    glm::vec3 ntl = nc + (up * Hnear / 2.0f) - (right * Wnear / 2.0f);
    glm::vec3 ntr = nc + (up * Hnear / 2.0f) + (right * Wnear / 2.0f);
    glm::vec3 nbl = nc - (up * Hnear / 2.0f) - (right * Wnear / 2.0f);
    glm::vec3 nbr = nc - (up * Hnear / 2.0f) + (right * Wnear / 2.0f);  

    g_FrustumCorners[0] = ftl;
    g_FrustumCorners[1] = ftr;
    g_FrustumCorners[2] = fbr;
    g_FrustumCorners[3] = fbl;
    g_FrustumCorners[4] = ntl;
    g_FrustumCorners[5] = ntr;
    g_FrustumCorners[6] = nbr;
    g_FrustumCorners[7] = nbl; 
/*
    #ifdef DEBUG
    LogMessage("Corner positions:");
    LogMessage("ftl: (%f, %f, %f)", ftl.x, ftl.y, ftl.z);
    LogMessage("ftr: (%f, %f, %f)", ftr.x, ftr.y, ftr.z);
    LogMessage("fbl: (%f, %f, %f)", fbl.x, fbl.y, fbl.z);
    LogMessage("fbr: (%f, %f, %f)", fbr.x, fbr.y, fbr.z);
    LogMessage("ntl: (%f, %f, %f)", ntl.x, ntl.y, ntl.z);
    LogMessage("ntr: (%f, %f, %f)", ntr.x, ntr.y, ntr.z);
    LogMessage("nbl: (%f, %f, %f)", nbl.x, nbl.y, nbl.z);
    LogMessage("nbr: (%f, %f, %f)", nbr.x, nbr.y, nbr.z);
    LogMessage("Near plane: %f Far plane: %f", nearPlane, farPlane);
    LogMessage("Fov: %f Aspect ratio: %f", glm::degrees(fov), aspectRatio);
    LogMessage("Cam Pos: (%f, %f, %f)", viewPos.x, viewPos.y, viewPos.z);
    printf("\n");
    #endif*/

    frustum.Planes[NEAR] = PlaneFromCorners(ntl, ntr, nbr);
    frustum.Planes[FAR] = PlaneFromCorners(ftr, ftl, fbl);
    frustum.Planes[BOTTOM] = PlaneFromCorners(nbl, nbr, fbr);
    frustum.Planes[TOP] = PlaneFromCorners(ntr, ntl, ftl);
    frustum.Planes[RIGHT] = PlaneFromCorners(nbr, ntr, fbr);
    frustum.Planes[LEFT] = PlaneFromCorners(ntl, nbl, fbl); 

    //for(int i = 0; i < NUM_PLANES; i++){
    //    LogMessage("Plane %d: (%f, %f, %f) %f", i, frustum.Planes[i].normal.x, frustum.Planes[i].normal.y, frustum.Planes[i].normal.z, frustum.Planes[i].distance);
    //}
}

float SignedDistanceToPlane(const Plane& plane, glm::vec3 position)
{
    return glm::dot(plane.normal, position) - plane.distance;
}

bool PointInFrustum(Frustum& frustum, glm::vec3 position)
{
    for(int i = 0; i < NUM_PLANES; i++){
        if(SignedDistanceToPlane(frustum.Planes[i], position) <= 0){ // point is behind plane
            return false;
        }
    }

    return true;
}

bool PointInFrustum(Frustum& frustum, float x, float y, float z)
{
    for(int i = 0; i < NUM_PLANES; i++){
        if(SignedDistanceToPlane(frustum.Planes[i], {x, y, z}) <= 0){ // point is behind plane
            return false;
        }
    }

    return true;
}

bool SphereInFrustum(Frustum& frustum, glm::vec3 position, float radius)
{
    for(int i = 0; i < NUM_PLANES; i++){
        if(SignedDistanceToPlane(frustum.Planes[i], position) < -radius){ // center is behind plane by more than the radius
            return false;
        }
    }

    return true;
}

bool isOnOrForwardPlane(const Plane& plane, glm::vec3 extents, glm::vec3 center) 
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
        extents.z * std::abs(plane.normal.z);

    return -r <= SignedDistanceToPlane(plane, center);
}

float GetSignedDistanceToPlane(const Plane& p, glm::vec3 point)
{
    return glm::dot(p.normal, point) - p.distance;
}

bool AABBInFrustum(Frustum& frustum, glm::vec3 min, glm::vec3 max)
{
    /*glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 extents = {max.x - center.x, max.y - center.y, max.z - center.z};

    return (
        isOnOrForwardPlane(frustum.Planes[LEFT], extents, center) &&
        isOnOrForwardPlane(frustum.Planes[RIGHT], extents, center) &&
        isOnOrForwardPlane(frustum.Planes[BOTTOM], extents, center) &&
        isOnOrForwardPlane(frustum.Planes[TOP], extents, center) &&
        isOnOrForwardPlane(frustum.Planes[NEAR], extents, center) &&
        isOnOrForwardPlane(frustum.Planes[FAR], extents, center)
    );*/

    for (const Plane& plane : frustum.Planes) {
        glm::vec3 positiveVertex = min;
        if (plane.normal.x >= 0) {
            positiveVertex.x = max.x;
        }
        if (plane.normal.y >= 0) {
            positiveVertex.y = max.y;
        }
        if (plane.normal.z >= 0) {
            positiveVertex.z = max.z;
        }
        if (GetSignedDistanceToPlane(plane, positiveVertex) < 0) {
            return false;
        }
    }
    return true;
}