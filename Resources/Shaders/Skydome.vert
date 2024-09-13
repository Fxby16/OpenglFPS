#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aPos; // Use position as texture coordinates
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Properly transform the vertex position
}
