#pragma once

#include <glm.hpp>
#include <cstdint>

extern void InitMousePicking();
extern void UpdateMousePicking();
extern void DeinitMousePicking();

/**
 * \returns a pair containing the id of the selected model and the index of the selected transform
 */
extern std::pair<uint32_t, unsigned int> GetSelectedModel(glm::vec2 mousePos);