#pragma once

#include <glm.hpp>

#include <shader.hpp>
#include <shadowmap.hpp>

struct PointLight{
    glm::vec3 pos;
    glm::vec3 color;
};

struct DirectionalLight{
    glm::vec3 dir;
    glm::vec3 color;
    ShadowMap shadowMap;
    glm::mat4 lightSpaceMatrix;

    DirectionalLight() = default;
    DirectionalLight(const glm::vec3& dir, const glm::vec3& color, const glm::mat4& lightSpaceMatrix)
        : dir(dir), color(color), lightSpaceMatrix(lightSpaceMatrix) {}
};

struct SpotLight{
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color;
    float cutOff;
    float outerCutOff;
};

extern void SetPointLight(const PointLight& pl);
extern void SetDirectionalLight(const DirectionalLight& dl);
extern void SetSpotLight(const SpotLight& dl);
extern void ResetCounters();