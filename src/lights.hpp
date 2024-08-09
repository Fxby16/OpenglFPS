#pragma once

#include <glm.hpp>
#include <shader.hpp>

struct PointLight{
    glm::vec3 pos;
    glm::vec3 color;
};

struct DirectionalLight{
    glm::vec3 dir;
    glm::vec3 color;
};

struct SpotLight{
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color;
    float cutOff;
    float outerCutOff;
};

extern void SetLightShader(Shader* shader);
extern void SetPointLight(const PointLight& pl);
extern void SetDirectionalLight(const DirectionalLight& dl);
extern void SetSpotLight(const SpotLight& dl);
extern void ResetCounters();