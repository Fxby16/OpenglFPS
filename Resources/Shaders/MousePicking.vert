#version 460 core

const int MAX_BONE_INFLUENCE = 4;
const int MAX_BONES = 100;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in ivec4 boneIDsIn;
layout (location = 5) in vec4 weightsIn;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform int isPlaying;

void main()
{
    float weights[MAX_BONE_INFLUENCE];
    weights[0] = weightsIn.x;
    weights[1] = weightsIn.y;
    weights[2] = weightsIn.z;
    weights[3] = weightsIn.w;

    int boneIDs[MAX_BONE_INFLUENCE];
    boneIDs[0] = boneIDsIn.x;
    boneIDs[1] = boneIDsIn.y;
    boneIDs[2] = boneIDsIn.z;
    boneIDs[3] = boneIDsIn.w;

    vec4 totalPosition = vec4(0.0f);

    if(isPlaying == 1){
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++){  
            if(boneIDs[i] == -1 && i == 0){                     //no bones for this vertex, just keep initial values
                totalPosition = vec4(vertexPosition, 1.0f);
                break;
            }
            
            if(boneIDs[i] >= MAX_BONES || boneIDs[i] == -1){    //nothing at this index, skip it
                continue;
            }

            vec4 localPosition = finalBonesMatrices[boneIDs[i]] * vec4(vertexPosition, 1.0f) * weights[i];
            totalPosition += localPosition;
        }
    }else{
        totalPosition = vec4(vertexPosition, 1.0f);
    }

    gl_Position = projection * view * model * totalPosition;
}