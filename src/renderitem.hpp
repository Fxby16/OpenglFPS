#pragma once

#include <cstdint>
#include <glm.hpp>

struct RenderItem{
    uint32_t model;
    glm::mat4 transform;

    void Draw();
};