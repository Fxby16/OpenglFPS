#pragma once

#include <glm.hpp>

struct AABB{
    glm::vec3 min;
    glm::vec3 max;

    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {}
};

struct OBB{
    glm::vec3 center;
    glm::vec3 extents;
    glm::mat3 rotation;

    OBB() = default;
    OBB(const glm::vec3& center, const glm::vec3& extents, const glm::mat3& rotation)
        : center(center), extents(extents), rotation(rotation) {}
};

OBB OBBFromAABB(const AABB& aabb, glm::mat4 modelMatrix);
AABB AABBFromOBB(const OBB& obb);