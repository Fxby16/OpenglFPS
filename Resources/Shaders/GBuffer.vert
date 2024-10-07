#version 460 core

const int MAX_BONE_INFLUENCE = 4;
const int MAX_BONES = 100;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in ivec4 boneIDsIn;
layout (location = 5) in vec4 weightsIn;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragTangent;
out vec3 fragBinormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool isPlaying;

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
    vec4 totalNormal = vec4(0.0f);
    vec4 totalTangent = vec4(0.0f);

    if(isPlaying){
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++){  
            if(boneIDs[i] == -1 && i == 0){                     //no bones for this vertex, just keep initial values
                totalPosition = vec4(vertexPosition, 1.0f);
                totalNormal = vec4(vertexNormal, 0.0f);
                totalTangent = vec4(vertexTangent, 0.0f);
                break;
            }
            
            if(boneIDs[i] >= MAX_BONES || boneIDs[i] == -1){    //nothing at this index, skip it
                continue;
            }

            vec4 localPosition = finalBonesMatrices[boneIDs[i]] * vec4(vertexPosition, 1.0f) * weights[i];
            vec4 localNormal = finalBonesMatrices[boneIDs[i]] * vec4(vertexNormal, 0.0f) * weights[i];
            vec4 localTangent = finalBonesMatrices[boneIDs[i]] * vec4(vertexTangent, 0.0f) * weights[i];

            totalPosition += localPosition;
            totalNormal += localNormal;
            totalTangent += localTangent;
        }
    }else{
        totalPosition = vec4(vertexPosition, 1.0f);
        totalNormal = vec4(vertexNormal, 0.0f);
        totalTangent = vec4(vertexTangent, 0.0f);
    }

    totalNormal.xyz = normalize(totalNormal.xyz);

    vec3 vertexBinormal = cross(totalNormal.xyz, totalTangent.xyz);
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    fragPosition = vec3(model * totalPosition);
    fragTexCoord = vertexTexCoord;
    fragNormal = normalize(normalMatrix * totalNormal.xyz);
    fragTangent = normalize(normalMatrix * totalTangent.xyz);
    fragTangent = normalize(fragTangent - dot(fragTangent, fragNormal) * fragNormal);
    fragBinormal = normalize(normalMatrix * vertexBinormal);
    fragBinormal = cross(fragNormal, fragTangent);

    gl_Position = projection * view * model * totalPosition;
}
