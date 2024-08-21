#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;

void main()
{
    float distance = length(FragPos.xyz - lightPos);
    distance = distance / 25.0;
    gl_FragDepth = distance;
}