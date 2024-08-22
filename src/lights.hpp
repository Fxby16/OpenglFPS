#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <shader.hpp>
#include <shadowmap.hpp>

struct PointLight{
    glm::vec3 pos;
    glm::vec3 color;
    PointLightShadowMap shadowMap;
    glm::mat4 lightSpaceMatrix[6];

    PointLight() = default;
    PointLight(const glm::vec3& pos, const glm::vec3& color)
        : pos(pos), color(color)
    {
        lightSpaceMatrix[0] = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f) * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        lightSpaceMatrix[1] = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f) * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        lightSpaceMatrix[2] = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f) * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        lightSpaceMatrix[3] = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f) * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        lightSpaceMatrix[4] = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f) * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        lightSpaceMatrix[5] = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f) * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    }
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
        lightSpaceMatrix = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 1.0f, 50.0f) * glm::lookAt(-dir * 20.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
        : pos(pos), dir(dir), color(color), cutOff(glm::cos(glm::radians(cutOff))), outerCutOff(glm::cos(glm::radians(outerCutOff)))
    {
        lightSpaceMatrix = glm::perspective(glm::radians(outerCutOff * 2), 1.0f, 0.1f, 30.0f) * glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

extern void SetPointLight(const PointLight& pl);
extern void SetDirectionalLight(const DirectionalLight& dl);
extern void SetSpotLight(const SpotLight& dl);
extern void ResetLightsCounters();

extern void DrawShadowMap(const DirectionalLight& light);
extern void DrawShadowMap(const PointLight& light);
extern void DrawShadowMap(const SpotLight& light);