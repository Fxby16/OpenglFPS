#pragma once

#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <functional>

extern int InitWindow(unsigned int width, unsigned int height, const char* title);
extern void CloseWindow();

extern GLFWwindow* GetWindow();

extern void ToggleVSync();
extern void EnableVSync();
extern void DisableVSync();
extern void SetVSync(bool vsync);
extern bool IsVSyncEnabled();
extern void EnableCursor();
extern void DisableCursor();
extern bool IsCursorEnabled();
extern void ToggleCursor();
extern double GetTime();
extern bool WindowShouldClose();
extern void SetWindowShouldClose();
extern void PollEvents();
extern void SwapBuffers();

extern void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
extern double GetScrollYDelta();

extern uint32_t AddWindowResizeCallback(std::function<void(int, int)> callback);
extern void RemoveWindowResizeCallback(uint32_t id);

extern std::vector<std::pair<int, int>> QueryAvailableResolutions();
extern void SetWindowResolution(int width, int height);

extern bool IsFullscreen();
extern void ToggleFullscreen();
extern void EnableFullscreen();
extern void DisableFullscreen();
extern void SetFullscreen(bool fullscreen);

extern bool IsKeyPressed(int key);
extern bool IsKeyReleased(int key);
extern bool IsKeyDown(int key);
extern bool IsKeyUp(int key);

extern bool IsMouseButtonPressed(int button);
extern bool IsMouseButtonReleased(int button);
extern bool IsMouseButtonDown(int button);
extern bool IsMouseButtonUp(int button);

extern glm::vec2 GetMousePosition();
extern int GetMouseXDelta();
extern int GetMouseYDelta();

extern void UpdateInputs();
extern void ResetLastMousePosition();