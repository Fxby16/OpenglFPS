#include <Camera.hpp>
#include <Globals.hpp>

static Camera g_Camera;

Camera& GetCamera()
{
    return g_Camera;
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Init(position, up, yaw, pitch);
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch)
{
    Init(pos_x, pos_y, pos_z, up_x, up_y, up_z, yaw, pitch);
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fovy)
{
    Init(position, target, up, fovy);    
}

void Camera::Init(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    m_Pos = position;
    m_WorldUp = up;
    m_Yaw = yaw;
    m_Pitch = pitch; 
    m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_MovementSpeed = 1.0f; 
    m_MouseSensitivity = 0.1f;
    m_Zoom = 45.0f;

    UpdateCameraVectors();
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Zoom), (float)g_ScreenWidth / (float)g_ScreenHeight, 0.1f, 100.0f);
}

void Camera::Init(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch)
{
    m_Pos = glm::vec3(pos_x, pos_y, pos_z);
    m_WorldUp = glm::vec3(up_x, up_y, up_z);
    m_Yaw = yaw;
    m_Pitch = pitch;
    m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_MovementSpeed = 2.5f;
    m_MouseSensitivity = 0.1f;
    m_Zoom = 45.0f;

    UpdateCameraVectors();
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Zoom), (float)g_ScreenWidth / (float)g_ScreenHeight, 0.1f, 100.0f);
}

void Camera::Init(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fovy)
{
    m_Pos = position;
    m_WorldUp = up;
    m_Yaw = -90.0f;
    m_Pitch = 0.0f; 
    m_Front = glm::normalize(target - position);
    m_MovementSpeed = 2.5f; 
    m_MouseSensitivity = 0.1f;
    m_Zoom = fovy;

    UpdateCameraVectors();
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Zoom), (float)g_ScreenWidth / (float)g_ScreenHeight, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(Movement direction, float delta_time)
{
    float velocity = m_MovementSpeed * delta_time;

    if(direction == Movement::FORWARD)
        m_Pos += m_Front * velocity;
    if(direction == Movement::BACKWARD)
        m_Pos -= m_Front * velocity;
    if(direction == Movement::LEFT)
        m_Pos -= m_Right * velocity;
    if(direction == Movement::RIGHT)
        m_Pos += m_Right * velocity;
    if(direction == Movement::UP)
        m_Pos += m_Up * velocity;
    if(direction == Movement::DOWN)
        m_Pos -= m_Up * velocity;
}

void Camera::ProcessMouseMovement(float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= m_MouseSensitivity;
    y_offset *= m_MouseSensitivity;

    m_Yaw += x_offset;
    m_Pitch += y_offset;

    if(constrain_pitch)
    {
        if(m_Pitch > 89.9f)
            m_Pitch = 89.9f;
        if(m_Pitch < -89.9f)
            m_Pitch = -89.9f;
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float y_offset)
{
    m_Zoom -= y_offset * 0.1f;
    m_Zoom = glm::clamp(m_Zoom, 1.0f, 90.0f);

    y_offset = 0.0f;

    m_ProjectionMatrix = glm::perspective(glm::radians(m_Zoom), (float)g_ScreenWidth / (float)g_ScreenHeight, 0.1f, 100.0f);
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;

    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);

    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}