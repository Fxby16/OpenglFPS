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
uniform int isCompressed;

void main(){
    vec3 albedo, normal;
    float metallic, roughness, ao;

    if(isCompressed == 1){
        albedo = texture(albedoMap, fragTexCoord).rgb;
        normal = texture(normalMap, fragTexCoord).rgb;
        metallic = texture(roughnessMap, fragTexCoord).b;
        roughness = texture(roughnessMap, fragTexCoord).g;
        ao = texture(roughnessMap, fragTexCoord).r;   
    }else{
        albedo = texture(albedoMap, fragTexCoord).rgb;
        normal = texture(normalMap, fragTexCoord).rgb;
        metallic = texture(metallicMap, fragTexCoord).r;
        roughness = texture(roughnessMap, fragTexCoord).r;
        ao = texture(aoMap, fragTexCoord).r;
    }

    mat3 TBN = transpose(mat3(fragTangent, fragBinormal, fragNormal));
    normal = normalize(TBN * (2.0 * normal - 1.0));

    if(usePBR == 1){
        PositionOut = vec4(fragPosition, roughness);
        NormalOut = vec4(normal, metallic);
        AlbedoOut = vec4(pow(texture(albedoMap, fragTexCoord).rgb, vec3(2.2)), texture(roughnessMap, fragTexCoord).r);
    }else{
        PositionOut = vec4(fragPosition, 1.0);
        NormalOut = vec4(normal, 1.0);
        AlbedoOut = vec4(pow(texture(albedoMap, fragTexCoord).rgb, vec3(2.2)), 1.0);
    }
}
