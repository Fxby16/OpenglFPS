#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Positions;
uniform sampler2D Normals;
uniform sampler2D Albedo;

uniform sampler2DArray ShadowMaps;
uniform samplerCubeArray ShadowCubeMaps;

uniform vec3 camPos;

const int MAX_LIGHTS = 10;

struct PointLight {
    vec3 position;
    vec3 color;
    int shadowMapIndex;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    int shadowMapIndex;
    mat4 lightSpaceMatrix;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    int shadowMapIndex;
    mat4 lightSpaceMatrix;
};

uniform PointLight pointLights[MAX_LIGHTS];
uniform DirectionalLight directionalLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform int numPointLights;
uniform int numDirectionalLights;
uniform int numSpotLights;

const float PI = 3.14159265359;

// return 0.0 if in shadow, 1.0 if not
float CalcShadow(int shadowMapIndex, vec4 fragPosLightSpace, vec3 lightDir, float minBias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(ShadowMaps, vec3(projCoords.xy, shadowMapIndex)).r;
    float currentDepth = projCoords.z;


    //PCF (Percentage-Closer Filtering) to soften shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(ShadowMaps, 0).xy);

    float bias = max(0.05 * (1.0 - dot(texture(Normals, TexCoords).rgb, lightDir)), minBias);
    
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            float pcfDepth = texture(ShadowMaps, vec3(projCoords.xy + vec2(x, y) * texelSize, shadowMapIndex)).r; 
            shadow += ((currentDepth - bias > pcfDepth) ? 0.0 : 1.0);        
        }    
    }
    shadow /= 9.0;
    
    //if outside of shadow map, consider not in shadow
    if(projCoords.z > 1.0)
        shadow = 1.0;

    return shadow;
}

float CalcShadowCube(PointLight pointLight, vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLight.position;
    float closestDepth = texture(ShadowCubeMaps, vec4(fragToLight, pointLight.shadowMapIndex)).r;
    closestDepth *= 25.0; 
    float currentDepth = length(fragToLight);

    // PCF (Percentage-Closer Filtering) to soften shadows
    float shadow = 0.0;
    float bias = 0.005;
    vec3 texelSize = 1.0 / vec3(textureSize(ShadowCubeMaps, 0).xy, textureSize(ShadowCubeMaps, 0).x);
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            for(int z = -1; z <= 1; z++){
                vec3 offset = vec3(x, y, z) * texelSize;
                float pcfDepth = texture(ShadowCubeMaps, vec4(fragToLight + offset, pointLight.shadowMapIndex)).r;
                pcfDepth *= 25.0; 
                shadow += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
            }
        }
    }
    shadow /= 27.0;

    return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 albedo = pow(texture(Albedo, TexCoords).rgb, vec3(2.2));
    vec3 normal = texture(Normals, TexCoords).rgb;
    vec3 position = texture(Positions, TexCoords).rgb;
    float roughness = texture(Positions, TexCoords).a;
    float metallic = texture(Normals, TexCoords).a;
    float ao = texture(Albedo, TexCoords).a;

    vec3 V = normalize(camPos - position);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // Point Lights
    for(int i = 0; i < numPointLights; i++) 
    {
        float shadow = CalcShadowCube(pointLights[i], position);

        vec3 L = normalize(pointLights[i].position - position);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;

        float NDF = DistributionGGX(normal, H, roughness);  
        float NDF2 = DistributionGGX(-normal, H, roughness); 
        float G   = GeometrySmith(normal, V, L, roughness); 
        float G2  = GeometrySmith(-normal, V, L, roughness);     
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; 
        vec3 specular = numerator / denominator;

        vec3 numerator2    = NDF2 * G2 * F;
        float denominator2 = 4.0 * max(dot(-normal, V), 0.0) * max(dot(-normal, L), 0.0) + 0.0001;
        vec3 specular2 = numerator2 / denominator2;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(normal, L), 0.0);     
        float NdotL2 = max(dot(-normal, L), 0.0);   

        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
        Lo += (kD * albedo / PI + specular2) * radiance * NdotL2 * shadow;
    }   

    // Directional Lights
    for(int i = 0; i < numDirectionalLights; i++) 
    {
        vec4 fragPosLightSpace = (directionalLights[i].lightSpaceMatrix * vec4(position, 1.0));
        float shadow = CalcShadow(directionalLights[i].shadowMapIndex, fragPosLightSpace, -directionalLights[i].direction, 0.005);

        vec3 L = normalize(-directionalLights[i].direction);
        vec3 H = normalize(V + L);
        vec3 radiance = directionalLights[i].color;

        float NDF = DistributionGGX(normal, H, roughness);   
        float NDF2 = DistributionGGX(-normal, H, roughness);
        float G   = GeometrySmith(normal, V, L, roughness); 
        float G2  = GeometrySmith(-normal, V, L, roughness);     
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        vec3 numerator2    = NDF2 * G2 * F;
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; 
        float denominator2 = 4.0 * max(dot(-normal, V), 0.0) * max(dot(-normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        vec3 specular2 = numerator2 / denominator2;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(normal, L), 0.0);   
        float NdotL2 = max(dot(-normal, L), 0.0);     

        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
        Lo += (kD * albedo / PI + specular2) * radiance * NdotL2 * shadow;
    }   

    // Spot Lights
    for(int i = 0; i < numSpotLights; i++) 
    {
        vec4 fragPosLightSpace = (spotLights[i].lightSpaceMatrix * vec4(position, 1.0));
        float shadow = CalcShadow(spotLights[i].shadowMapIndex, fragPosLightSpace, -spotLights[i].direction, 0.002);

        vec3 L = normalize(spotLights[i].position - position);
        vec3 H = normalize(V + L);
        float distance = length(spotLights[i].position - position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = spotLights[i].color * attenuation;

        float theta = dot(L, normalize(-spotLights[i].direction));
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
        radiance *= intensity;

        float NDF = DistributionGGX(normal, H, roughness);   
        float NDF2 = DistributionGGX(-normal, H, roughness);
        float G   = GeometrySmith(normal, V, L, roughness); 
        float G2  = GeometrySmith(-normal, V, L, roughness);     
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        vec3 numerator2    = NDF2 * G2 * F;
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; 
        float denominator2 = 4.0 * max(dot(-normal, V), 0.0) * max(dot(-normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        vec3 specular2 = numerator2 / denominator2;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(normal, L), 0.0);       
        float NdotL2 = max(dot(-normal, L), 0.0); 

        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
        Lo += (kD * albedo / PI + specular2) * radiance * NdotL2 * shadow;
    }
    
    // ambient lighting
    vec3 ambient = vec3(0.03) * albedo;
    
    vec3 color = ambient + Lo;

    FragColor = vec4(color, 1.0);
}