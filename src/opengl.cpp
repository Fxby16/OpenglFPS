#include <opengl.hpp>
#include <raylib.h>

void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(type == GL_DEBUG_TYPE_ERROR){
        TraceLog(LOG_ERROR, "OpenGL error: %s (source: %d, type: %d, id: %d, severity: %d)", message, source, type, id, severity);
    }
}