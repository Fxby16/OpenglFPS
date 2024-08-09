#pragma once

#include <glm.hpp>

struct BoundingBox{
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox() = default;
    BoundingBox(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {}
};
    