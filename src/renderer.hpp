#pragma once

#include <raylib.h>

struct GBuffer{
    unsigned int framebuffer;

    unsigned int positionTexture;
    unsigned int normalTexture;
    unsigned int albedoSpecTexture;

    unsigned int depthRenderbuffer;
};

enum DeferredMode{
   DEFERRED_POSITION,
   DEFERRED_NORMAL,
   DEFERRED_ALBEDO,
   DEFERRED_SHADING
};

extern void DeferredPass(GBuffer& gBuffer, Shader& deferredShader, Camera& camera, DeferredMode deferredMode);