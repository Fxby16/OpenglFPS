#version 460 core

layout (location = 0) out vec4 PositionOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 AlbedoOut;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in vec3 fragBinormal;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform int usePBR;

void main() {
    mat3 TBN = transpose(mat3(fragTangent, fragBinormal, fragNormal));
    vec3 normal = normalize(TBN * (2.0 * texture(normalMap, fragTexCoord).rgb - 1.0));

    if(usePBR == 1){
        PositionOut = vec4(fragPosition, texture(roughnessMap, fragTexCoord).g);
        NormalOut = vec4(normal, texture(roughnessMap, fragTexCoord).b);
        AlbedoOut = pow(vec4(texture(albedoMap, fragTexCoord).rgb, texture(roughnessMap, fragTexCoord).r), vec4(2.2));
    }else{
        PositionOut = vec4(fragPosition, 1.0);
        NormalOut = vec4(normal, 1.0);
        AlbedoOut = pow(vec4(texture(albedoMap, fragTexCoord).rgb, 1.0), vec4(2.2));
    }
}
