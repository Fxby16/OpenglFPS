#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <Window.hpp>
#include <Globals.hpp>
#include <PostProcessing.hpp>

#include <string>
#include <vector>

static bool showSettingsMenu = false;
static std::vector<const char*> resolutions;
static int resolutionIndex = 0;

bool GetShowSettingsMenu()
{
    return showSettingsMenu;
}

void SetShowSettingsMenu(bool show)
{
    showSettingsMenu = show;
}

void InitSettingsMenu()
{
    std::vector<std::pair<int, int>> res = QueryAvailableResolutions();

    resolutions.clear();

    char buffer[30];
    for(auto& [width, height] : res){
        if(width == g_ScreenWidth && height == g_ScreenHeight){
            resolutionIndex = resolutions.size();
        }
        
        sprintf(buffer, "%dx%d", width, height);

        resolutions.push_back(strdup(buffer));
    }
}

void DeinitSettingsMenu()
{
    for(auto& res : resolutions){
        free((void*)res);
    }

    resolutions.clear();
}

void SettingsMenu()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    // make the window fullscreen
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(displaySize);

    if(ImGui::Begin("Settings Menu", nullptr, windowFlags)){
        if(ImGui::BeginTabBar("SettingsTabBar")){
            if(ImGui::BeginTabItem("Video")){
                bool fullscreen = IsFullscreen();
                if(ImGui::Checkbox("Fullscreen", &fullscreen)){
                    SetFullscreen(fullscreen);
                }

                bool vsync = IsVSyncEnabled();
                if(ImGui::Checkbox("VSync", &vsync)){
                    SetVSync(vsync);
                }

                if(ImGui::Combo("Resolution", &resolutionIndex, resolutions.data(), resolutions.size())){
                    int width, height;
                    sscanf(resolutions[resolutionIndex], "%dx%d", &width, &height);
                    SetWindowResolution(width, height);
                }

                ImGui::EndTabItem();
            }

            if(ImGui::BeginTabItem("Graphics")){
                bool useBloom = GetUseBloom();
                if(ImGui::Checkbox("Bloom", &useBloom)){
                    UseBloom(useBloom);
                }   
            }

            if (ImGui::BeginTabItem("Audio")){
                ImGui::EndTabItem();
            }

            if(ImGui::BeginTabItem("Game")){
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(100, 30);
        ImGui::SetCursorPos(ImVec2(windowSize.x - buttonSize.x - 10, windowSize.y - buttonSize.y - 10)); 

        if(ImGui::Button("Close", buttonSize)){
            showSettingsMenu = false;
            DisableCursor();
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
