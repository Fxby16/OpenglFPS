#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <shader.hpp>
#include <shadowmap.hpp>

struct PointLight{
    glm::vec3 pos;
    glm::vec3 color;
    PointLightShadowMap shadowMap;
    glm::mat4 lightSpaceMatrix;

    PointLight() = default;
    PointLight(const glm::vec3& pos, const glm::vec3& color)
        : pos(pos), color(color) {}
};

struct DirectionalLight{
    glm::vec3 dir;
    glm::vec3 color;
    ShadowMap shadowMap;
    glm::mat4 lightSpaceMatrix;

    DirectionalLight() = default;
    DirectionalLight(const glm::vec3& dir, const glm::vec3& color)
        : dir(dir), color(color)
    {
        lightSpaceMatrix = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 10.0f, 35.0f) * glm::lookAt(-dir * 10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

struct SpotLight{
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color;
    float cutOff;
    float outerCutOff;
    ShadowMap shadowMap;
    glm::mat4 lightSpaceMatrix;

    SpotLight() = default;
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, float cutOff, float outerCutOff)
        : pos(pos), dir(dir), color(color), cutOff(cutOff), outerCutOff(outerCutOff)
    {
        lightSpaceMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 30.0f) * glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

extern void SetPointLight(const PointLight& pl);
extern void SetDirectionalLight(const DirectionalLight& dl);
extern void SetSpotLight(const SpotLight& dl);
extern void ResetCounters();