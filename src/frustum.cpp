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

glm::vec3 PlaneIntersection(const glm::vec4& p1, const glm::vec4& p2, const glm::vec4& p3)
{
    glm::mat4 mat = {
        p1.x, p1.y, p1.z, 0,
        p2.x, p2.y, p2.z, 0,
        p3.x, p3.y, p3.z, 0,
        0, 0, 0, 1
    };
    glm::vec4 point = {
        -p1.w,
        -p2.w,
        -p3.w,
        1
    };
    glm::mat4 invMat = glm::inverse(mat);
    glm::vec3 result = invMat * point;
    return result;
}

void ExtractFrustumCorners(const Frustum& frustum, glm::vec3 corners[8])
{
    corners[0] = PlaneIntersection(frustum.Planes[LEFT], frustum.Planes[TOP], frustum.Planes[FAR]);
    corners[1] = PlaneIntersection(frustum.Planes[RIGHT], frustum.Planes[TOP], frustum.Planes[FAR]);
    corners[2] = PlaneIntersection(frustum.Planes[RIGHT], frustum.Planes[BOTTOM], frustum.Planes[FAR]);
    corners[3] = PlaneIntersection(frustum.Planes[LEFT], frustum.Planes[BOTTOM], frustum.Planes[FAR]);
    corners[4] = PlaneIntersection(frustum.Planes[LEFT], frustum.Planes[TOP], frustum.Planes[NEAR]);
    corners[5] = PlaneIntersection(frustum.Planes[RIGHT], frustum.Planes[TOP], frustum.Planes[NEAR]);
    corners[6] = PlaneIntersection(frustum.Planes[RIGHT], frustum.Planes[BOTTOM], frustum.Planes[NEAR]);
    corners[7] = PlaneIntersection(frustum.Planes[LEFT], frustum.Planes[BOTTOM], frustum.Planes[NEAR]);
}

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
    DrawLine3D(g_FrustumCorners[1], g_FrustumCorners[5], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine3D(g_FrustumCorners[2], g_FrustumCorners[6], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
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

void SetPlane(glm::vec4& plane, glm::vec3 point, glm::vec3 normal)
{
    normal = glm::normalize(normal);

    plane.x = normal.x;
    plane.y = normal.y;
    plane.z = normal.z;
    plane.w = -glm::dot(normal, point);

    #ifdef DEBUG
    if(normal.x * point.x + normal.y * point.y + normal.z * point.z + plane.w != 0){
        printf("Plane not set correctly\n");
    }

    LogMessage("Normal: (%f, %f, %f)", plane.x, plane.y, plane.z);
    LogMessage("Distance: %f", plane.w);
    #endif
}

glm::vec4 PlaneFromCorners(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);
    glm::vec4 plane;
    SetPlane(plane, p1, normal);
    return plane;
}

void ExtractFrustum(Frustum& frustum, const Camera& camera)
{
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 projectionMatrix = camera.GetProjectionMatrix();

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
    #endif

    frustum.Planes[NEAR] = PlaneFromCorners(ntr, ntl, nbl);
    frustum.Planes[FAR] = PlaneFromCorners(ftr, ftl, fbl);
    frustum.Planes[BOTTOM] = PlaneFromCorners(nbl, nbr, fbr);
    frustum.Planes[TOP] = PlaneFromCorners(ntl, ntr, ftr);
    frustum.Planes[RIGHT] = PlaneFromCorners(ntr, nbr, fbr);
    frustum.Planes[LEFT] = PlaneFromCorners(ntl, nbl, fbl); 
}

float SignedDistanceToPlane(const glm::vec4& plane, const glm::vec3& position)
{
    #ifdef DEBUG
    LogMessage("Distance to plane: %f", glm::dot(glm::vec3(plane.x, plane.y, plane.z), position) - plane.w);
    #endif
    return glm::dot(glm::vec3(plane.x, plane.y, plane.z), position) - plane.w;
}

float SignedDistanceToPlane(const glm::vec4& plane, float x, float y, float z)
{
    #ifdef DEBUG
    LogMessage("Distance to plane: %f", glm::dot(glm::vec3(plane.x, plane.y, plane.z), glm::vec3(x, y, z)) - plane.w);
    #endif
    return glm::dot(glm::vec3(plane.x, plane.y, plane.z), glm::vec3(x, y, z)) - plane.w;
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
        if(SignedDistanceToPlane(frustum.Planes[i], x, y, z) <= 0){ // point is behind plane
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

glm::vec3 GetPlaneNormal(const glm::vec4& plane)
{
    return glm::vec3(plane.x, plane.y, plane.z);
}

glm::vec3 GetPlanePoint(const glm::vec4& plane)
{
    return -GetPlaneNormal(plane) * plane.w;
}

bool AABBInFrustum(Frustum& frustum, glm::vec3 min, glm::vec3 max)
{
    for (int i = 0; i < NUM_PLANES; i++) {
        glm::vec3 normal = GetPlaneNormal(frustum.Planes[i]);

        glm::vec3 positiveVertex = min;
        glm::vec3 negativeVertex = max;

        if (normal.x >= 0) {
            positiveVertex.x = max.x;
            negativeVertex.x = min.x;
        }
        if (normal.y >= 0) {
            positiveVertex.y = max.y;
            negativeVertex.y = min.y;
        }
        if (normal.z >= 0) {
            positiveVertex.z = max.z;
            negativeVertex.z = min.z;
        }

        if (glm::dot(normal, positiveVertex) + frustum.Planes[i].w < 0) {
            return false;
        }
        if (glm::dot(normal, negativeVertex) + frustum.Planes[i].w <= 0) {
            // AABB is completely outside of the frustum plane
            return false;
        }
    }

    return true;
}