#pragma once

#include <GLFW/glfw3.h>
#include <glm.hpp>

extern int InitWindow(unsigned int width, unsigned int height, const char* title);
extern void CloseWindow();

extern GLFWwindow* GetWindow();

extern void ToggleVSync();
extern void EnableVSync();
extern void DisableVSync();
extern bool IsVSyncEnabled();
extern void EnableCursor();
extern void DisableCursor();
extern bool IsCursorEnabled();
extern double GetTime();
extern bool WindowShouldClose();
extern void SetWindowShouldClose();
extern void PollEvents();
extern void SwapBuffers();

extern void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
extern double GetScrollYDelta();

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