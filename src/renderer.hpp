#pragma once

#include <shader.hpp>
#include <camera.hpp>
#include <gbuffer.hpp>
#include <bounding_box.hpp>
#include <framebuffer.hpp>

enum DeferredMode{
   DEFERRED_POSITION,
   DEFERRED_NORMAL,
   DEFERRED_ALBEDO,
   DEFERRED_SHADING
};

extern void UpdateProj(glm::mat4 proj);
extern void UpdateView(glm::mat4 view);

extern void InitRenderer();
extern void DeinitRenderer();
extern void DeferredPass(GBuffer& gBuffer, Shader& deferredShader, Camera& camera, DeferredMode deferredMode);
extern void DrawFullscreenQuad();

extern Framebuffer& GetDeferredPassFramebuffer();

extern void DrawLine3D(glm::vec3 start, glm::vec3 end, glm::vec4 color);
extern void DrawBoundingBox(glm::vec3 min, glm::vec3 max, glm::vec4 color);
extern void DrawBoundingBox(const AABB& box, glm::vec4 color);

/**
 * @brief Parameters in OpenGL coordinates ([-1, 1] range)
 */
extern void DrawTexture(unsigned int texture, float x, float y, float width, float height);

extern void DrawSolidCube(glm::vec3 position, glm::vec3 scale, glm::vec4 color);