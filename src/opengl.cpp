#include <opengl.hpp>
#include <log.hpp>

void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(type == GL_DEBUG_TYPE_ERROR){
        LogError("OpenGL error: %s (source: %d, type: %d, id: %d, severity: %d)", message, source, type, id, severity);
    }
}

void OpenglFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void EnableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}

void DisableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}

void ClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EnableColorBlend()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DisableColorBlend()
{
    glDisable(GL_BLEND);
}

void BindTexture(unsigned int texture, int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void BindTextureArray(unsigned int textureArray, int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
}

void BindFramebuffer(int target, unsigned int framebuffer)
{
    glBindFramebuffer(target, framebuffer);
}

void UnbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, unsigned int mask)
{
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, ((mask & GL_DEPTH_BUFFER_BIT) || (mask & GL_STENCIL_BUFFER_BIT)) ? GL_NEAREST : GL_LINEAR);
}