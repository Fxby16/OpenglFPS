#include <Globals.hpp>
#include <Log.hpp>
#include <ResourceManager.hpp>

#include <glad/glad.h>

static unsigned int g_FBO;
static unsigned int g_ColorTexture;
static unsigned int g_DepthBuffer;
static uint32_t g_MousePickingShader;

void InitMousePicking()
{
    glGenFramebuffers(1, &g_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);

    glGenTextures(1, &g_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16UI, g_ScreenWidth, g_ScreenHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_ColorTexture, 0);

    glGenRenderbuffers(1, &g_DepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, g_DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_ScreenWidth, g_ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_DepthBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LogError("Mouse picking framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    g_MousePickingShader = LoadShader("Resources/Shaders/MousePicking.vert", "Resources/Shaders/MousePicking.frag");
    GetShader(g_MousePickingShader)->Bind();
    GetShader(g_MousePickingShader)->SetUniform1i("isPlaying", 0);
    GetShader(g_MousePickingShader)->SetUniformMat4fv("projection", GetCamera().GetProjectionMatrix());
}

void DeinitMousePicking()
{
    glDeleteFramebuffers(1, &g_FBO);
    glDeleteTextures(1, &g_ColorTexture);
    glDeleteRenderbuffers(1, &g_DepthBuffer);
}

void UpdateMousePicking()
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GetShader(g_MousePickingShader)->Bind();

    auto& models = GetModels();

    for(auto& [id, model] : models){
        GetShader(g_MousePickingShader)->SetUniform1ui("id", id);

        auto& transforms = model.GetTransforms();
        for(unsigned int i = 0; i < transforms.size(); i++){
            GetShader(g_MousePickingShader)->SetUniform1ui("transform_index", i);
            model.DrawDepth(*GetShader(g_MousePickingShader), GetCamera().GetViewMatrix(), transforms[i]);
        }
    }

    auto& skinned_models = GetSkinnedModels();

    for(auto& [id, skinned_model] : skinned_models){
        GetShader(g_MousePickingShader)->SetUniform1ui("id", id);

        auto& transforms = skinned_model.model.GetTransforms();
        for(unsigned int i = 0; i < transforms.size(); i++){
            GetShader(g_MousePickingShader)->SetUniform1ui("transform_index", i);
            skinned_model.animator.UploadFinalBoneMatrices(*GetShader(g_MousePickingShader));
            skinned_model.model.DrawDepth(*GetShader(g_MousePickingShader), GetCamera().GetViewMatrix(), transforms[i]);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::pair<uint32_t, unsigned int> GetSelectedModel(glm::vec2 mousePos)
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);

    unsigned short pixel[3];
    glReadPixels(mousePos.x, mousePos.y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, pixel);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0){
        return {std::numeric_limits<uint32_t>::max(), 0};
    }

    return {(pixel[0] << 16) | pixel[1], pixel[2]};
}