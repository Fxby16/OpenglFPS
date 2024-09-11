#version 460 core

uniform uint id;
uniform uint transform_index;

out uvec3 data;

void main(){
    uvec3 temp = uvec3(0);
    temp.x = (id >> 16) & 0xFFFF;
    temp.y = id & 0xFFFF;
    temp.z = transform_index;

    data = temp;
}