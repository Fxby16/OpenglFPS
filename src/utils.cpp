#include <utils.hpp>
#include <text.hpp>
#include <glfw.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image_write.h>
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