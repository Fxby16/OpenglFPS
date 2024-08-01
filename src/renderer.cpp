#include <renderer.hpp>
#include <globals.hpp>

#include <rlgl.h>
#include <raymath.h>

extern void DeferredPass(GBuffer& gBuffer, Shader& deferredShader, Camera& camera, DeferredMode deferredMode)
{
    switch (deferredMode){
        case DEFERRED_SHADING:
        {
            BeginMode3D(camera);
                rlDisableColorBlend();
                rlDisableDepthTest();
                rlEnableShader(deferredShader.id);

                    rlActiveTextureSlot(0);
                    rlEnableTexture(gBuffer.positionTexture);
                    rlActiveTextureSlot(1);
                    rlEnableTexture(gBuffer.normalTexture);
                    rlActiveTextureSlot(2);
                    rlEnableTexture(gBuffer.albedoSpecTexture);

                    rlLoadDrawQuad();
                rlDisableShader();
                rlEnableDepthTest();
                rlEnableColorBlend();
            EndMode3D();

            // As a last step, we now copy over the depth buffer from our g-buffer to the default framebuffer.
            rlBindFramebuffer(RL_READ_FRAMEBUFFER, gBuffer.framebuffer);
            rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, 0);
            rlBlitFramebuffer(0, 0, g_ScreenWidth, g_ScreenHeight, 0, 0, g_ScreenWidth, g_ScreenHeight, 0x00000100);    // GL_DEPTH_BUFFER_BIT
            rlDisableFramebuffer();

            //BeginMode3D(camera);
            //EndMode3D();

        } break;
        case DEFERRED_POSITION:
        {
            DrawTextureRec((Texture2D){
                .id = gBuffer.positionTexture,
                .width = g_ScreenWidth,
                .height = g_ScreenHeight,
            }, (Rectangle) { 0, 0, (float)g_ScreenWidth, (float)-g_ScreenHeight }, Vector2Zero(), RAYWHITE);
        } break;
        case DEFERRED_NORMAL:
        {
            DrawTextureRec((Texture2D){
                .id = gBuffer.normalTexture,
                .width = g_ScreenWidth,
                .height = g_ScreenHeight,
            }, (Rectangle) { 0, 0, (float)g_ScreenWidth, (float)-g_ScreenHeight }, Vector2Zero(), RAYWHITE);
        } break;
        case DEFERRED_ALBEDO:
        {
            DrawTextureRec((Texture2D){
                .id = gBuffer.albedoSpecTexture,
                .width = g_ScreenWidth,
                .height = g_ScreenHeight,
            }, (Rectangle) { 0, 0, (float)g_ScreenWidth, (float)-g_ScreenHeight }, Vector2Zero(), RAYWHITE);
        } break;
        default: break;
    }
}