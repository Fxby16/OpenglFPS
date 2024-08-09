#pragma once

#include <unordered_map>
#include <glm.hpp>

struct KeyState{
    KeyState()
    {
        current = previous = 0;
    }

    bool current;
    bool previous;
};

class Input{
public:
    Input() = default;

    void UpdateStates();

    inline KeyState GetKey(int key){ return m_KeyStates[key]; }
    inline KeyState GetMouseButton(int button){ return m_MouseStates[button]; }
    inline glm::vec2 GetMousePosition(){ return m_MousePosition; }

    inline int GetMouseXDelta(){ return m_MousePosition.x - m_PreviousMousePosition.x; }
    inline int GetMouseYDelta(){ return m_MousePosition.y - m_PreviousMousePosition.y; }

private:
    std::unordered_map<int, KeyState> m_KeyStates;
    std::unordered_map<int, KeyState> m_MouseStates;
    glm::vec2 m_MousePosition;
    glm::vec2 m_PreviousMousePosition;
};