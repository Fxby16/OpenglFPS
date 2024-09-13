#pragma once

#include <string>
#include <glm.hpp>

extern void LoadSkydome(const std::string& hdri);
extern void UnloadSkydome();
extern void DrawSkydome(glm::mat4 view, glm::mat4 projection, glm::mat4 model);