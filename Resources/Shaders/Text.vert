#version 460 core
layout(location = 0) in vec2 vertex;

out vec2 TexCoords;
out int index;

uniform mat4 transforms[100];
uniform mat4 proj;

void main(){
    gl_Position = proj * transforms[gl_InstanceID] * vec4(vertex.xy, 0.0, 1.0);
    index = gl_InstanceID;
    TexCoords = vertex.xy;
    TexCoords.y = 1.0f - TexCoords.y;
}