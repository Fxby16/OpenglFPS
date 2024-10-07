#version 460 core

layout (location = 0) out vec4 PositionOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 AlbedoOut;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in vec3 fragBinormal;

#define ALBEDO 0
#define NORMAL 1
#define ROUGHNESS 2
#define METALLIC 3
#define AO 4

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform bool hasTextures[5];

void main(){
    vec3 albedo, normal;
    float metallic, roughness, ao;

    if(hasTextures[ROUGHNESS] && !hasTextures[METALLIC]){       //pbr material with attributes compressed in roughness texture
        albedo = texture(albedoMap, fragTexCoord).rgb;
        normal = texture(normalMap, fragTexCoord).rgb;
        metallic = texture(roughnessMap, fragTexCoord).b;
        roughness = texture(roughnessMap, fragTexCoord).g;
        ao = texture(roughnessMap, fragTexCoord).r;   
    }else if(hasTextures[ROUGHNESS] && hasTextures[METALLIC]){ //pbr material with a texture for every attribute
        albedo = texture(albedoMap, fragTexCoord).rgb;
        normal = texture(normalMap, fragTexCoord).rgb;
        metallic = texture(metallicMap, fragTexCoord).r;
        roughness = texture(roughnessMap, fragTexCoord).r;
        ao = texture(aoMap, fragTexCoord).r;
    }else{                                                               //non pbr material that has only diffuse and normal
        albedo = texture(albedoMap, fragTexCoord).rgb;
        normal = texture(normalMap, fragTexCoord).rgb;
        metallic = 0.0;
        roughness = 0.5;
        ao = 1.0;
    }

    mat3 TBN = transpose(mat3(fragTangent, fragBinormal, fragNormal));
    normal = normalize(TBN * (2.0 * normal - 1.0));

    PositionOut = vec4(fragPosition, roughness);
    NormalOut = vec4(normal, metallic);
    AlbedoOut = vec4(pow(texture(albedoMap, fragTexCoord).rgb, vec3(2.2)), ao);
}
