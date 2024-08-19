#include <bounding_box.hpp>

OBB OBBFromAABB(const AABB& aabb, glm::mat4 modelMatrix)
{
    glm::vec3 min = aabb.min;
    glm::vec3 max = aabb.max;

    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, max.y, max.z)
    };

    glm::vec3 transformedCorners[8];
    for (int i = 0; i < 8; i++) {
        transformedCorners[i] = glm::vec3(modelMatrix * glm::vec4(corners[i], 1.0f));
    }

    glm::vec3 center = glm::vec3(0.0f);
    for (int i = 0; i < 8; i++) {
        center += transformedCorners[i];
    }
    center /= 8.0f;

    glm::mat3 rotation = glm::mat3(modelMatrix);

    glm::vec3 extents = glm::vec3(0.0f);
    for (int i = 0; i < 8; i++) {
        glm::vec3 offset = transformedCorners[i] - center;
        for (int j = 0; j < 3; j++) {  // Loop through the 3 principal axes
            float projection = glm::dot(offset, rotation[j]);
            extents[j] = glm::max(extents[j], glm::abs(projection));
        }
    }

    return OBB(center, extents, rotation);
}

AABB AABBFromOBB(const OBB& obb)
{
    glm::vec3 min = obb.center - obb.extents;
    glm::vec3 max = obb.center + obb.extents;

    return AABB(min, max);
}