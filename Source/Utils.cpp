#include <Utils.hpp>
#include <Text.hpp>
#include <Window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image_write.h>
#include <glm.hpp>

#include <filesystem>

void DrawFrameTime(double value, int posX, int posY)
{
    glm::vec4 color = {0.0f, 1.0f, 0.0f, 1.0f};                             // Good FPS

    if(value > 1.0f / 20.0f) color = {1.0f, 0.0f, 0.0f, 1.0f};              // Warning FPS
    else if(value > 1.0f / 40.0f) color = {1.0f, 0.5f, 0.0f, 1.0f};         // Critical FPS

    DrawText(FormatText("%.2f ms", value * 1000), posX, posY, 1, color);
}

void DrawFPS(double value, int posX, int posY)
{
    glm::vec4 color = {0.0f, 1.0f, 0.0f, 1.0f};                             // Good FPS

    if(value < 20.0f) color = {1.0f, 0.0f, 0.0f, 1.0f};                     // Warning FPS
    else if(value < 40.0f) color = {1.0f, 0.5f, 0.0f, 1.0f};                // Critical FPS

    DrawText(FormatText("%.2f FPS", value), posX, posY, 1, color);
}

void TakeScreenshot()
{
    int width, height;
    glfwGetFramebufferSize(GetWindow(), &width, &height);

    unsigned char *pixels = new unsigned char[width * height * 3];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    std::filesystem::path screenshotPath = std::filesystem::current_path() / "screenshot";
    int n = 0;
    do{
        std::filesystem::path tempPath = screenshotPath.string() + std::to_string(n) + ".png"; 
        if(!std::filesystem::exists(tempPath))
        {
            screenshotPath = tempPath;
            break;
        }
    }while(++n < 100);

    stbi_flip_vertically_on_write(true);
    stbi_write_png(screenshotPath.string().c_str(), width, height, 3, pixels, width * 3);

    delete[] pixels;
}

glm::mat4 AiToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    return to;
}