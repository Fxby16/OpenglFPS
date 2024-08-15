#include <renderitem.hpp>
#include <resource_manager.hpp>
#include <camera.hpp>

void RenderItem::Draw()
{
    GetModel(model).Draw(GetGBufferShader(), GetCamera().GetViewMatrix(), model);
}