#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
extern void OpenglFramebufferSizeCallback(GLFWwindow* window, int width, int height);
extern void EnableDepthTest();
extern void DisableDepthTest();
extern void ClearColor(float r, float g, float b, float a);
extern void ClearScreen();
extern void EnableColorBlend();
extern void DisableColorBlend();
extern void BindTexture(unsigned int texture, int slot);
extern void BindTextureArray(unsigned int textureArray, int slot);
extern void BindFramebuffer(int target, unsigned int framebuffer);
extern void UnbindFramebuffer();
extern void BlitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, unsigned int mask);