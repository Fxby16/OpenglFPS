#include <input.hpp>
#include <globals.hpp>
#include <glfw.hpp>
#include <GLFW/glfw3.h>

void Input::UpdateStates(){
    for(auto& [key, keystate] : m_KeyStates){
        keystate.previous = keystate.current;
        keystate.current = glfwGetKey(GetWindow(), key);
    }

    for(auto& [button, keystate] : m_MouseStates){
        keystate.previous = keystate.current;
        keystate.current = glfwGetMouseButton(GetWindow(), button);
    }

    m_PreviousMousePosition = m_MousePosition;

    double x, y;
    glfwGetCursorPos(GetWindow(), &x, &y);
    m_MousePosition = glm::vec2(x, g_ScreenHeight - y);
}