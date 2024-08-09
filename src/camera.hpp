#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <globals.hpp>

class Camera{
public:
    enum class Movement{
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
    Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fovy);

    void Init(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
    void Init(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);
    void Init(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fovy);

    void ProcessKeyboard(Movement direction, float delta_time);
    void ProcessMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true);
    void ProcessMouseScroll(float y_offset);

    inline glm::vec3 GetPosition() const { return m_Pos; }
    inline glm::vec3 GetFront() const { return m_Front; }
    inline glm::vec3 GetUp() const { return m_Up; }
    inline glm::vec3 GetRight() const { return m_Right; }
    inline glm::vec3 GetWorldUp() const { return m_WorldUp; }
    inline glm::vec3 GetTarget() const { return m_Pos + m_Front; }
    inline float GetYaw() const { return m_Yaw; }
    inline float GetPitch() const { return m_Pitch; }
    inline float GetZoom() const { return m_Zoom; }
    inline glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
    inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);}

private:
    void UpdateCameraVectors();

    glm::vec3 m_Pos;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    
    float m_Yaw;
    float m_Pitch;
    
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;

    glm::mat4 m_ProjectionMatrix;

public:
    float m_nh = glm::tan(glm::radians(g_FOV * 0.5f)) * g_Near;
    float m_nw = m_nh * g_AspectRatio;
    float m_fh = glm::tan(glm::radians(g_FOV * 0.5f)) * g_Far;
    float m_fw = m_fh * g_AspectRatio;
};