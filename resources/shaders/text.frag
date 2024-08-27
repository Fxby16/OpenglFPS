#version 460 core

layout(location = 0) out vec4 color;

in vec2 TexCoords;
in flat int index;

uniform sampler2DArray text;
uniform int letterMap[100];
uniform vec4 textColor;

void main(){    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vec3(TexCoords.xy, letterMap[index])).r);
    color = textColor * sampled;
}