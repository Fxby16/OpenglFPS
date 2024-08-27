#version 460 core

const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;
layout (location = 5) in int boneIDs[MAX_BONE_INFLUENCE];
layout (location = 6) in int weights[MAX_BONE_INFLUENCE];

out vec4 FragPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    FragPos = model * vec4(vertexPosition, 1.0);
    gl_Position = lightSpaceMatrix * model * vec4(vertexPosition, 1.0);
}