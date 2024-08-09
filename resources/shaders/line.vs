#version 460 core

layout(location = 0) in vec3 vertexPos;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * vec4(vertexPos, 1.0);
}