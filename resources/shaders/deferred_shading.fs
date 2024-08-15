#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Positions;
uniform sampler2D Normals;
uniform sampler2D Albedo;

uniform vec3 camPos;

const int MAX_LIGHTS = 16;

struct PointLight {
    vec3 position;
    vec3 color;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
};

uniform PointLight pointLights[MAX_LIGHTS];
uniform DirectionalLight directionalLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform int numPointLights;
uniform int numDirectionalLights;
uniform int numSpotLights;

const float PI = 3.14159265359;

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

vec3 UnchartedTonemap(vec3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
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
        vec3 L = normalize(pointLights[i].position - position);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;

        float NDF = DistributionGGX(normal, H, roughness);   
        float G   = GeometrySmith(normal, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; 
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(normal, L), 0.0);        

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   

    // Directional Lights
    for(int i = 0; i < numDirectionalLights; i++) 
    {
        vec3 L = normalize(-directionalLights[i].direction);
        vec3 H = normalize(V + L);
        vec3 radiance = directionalLights[i].color;

        float NDF = DistributionGGX(normal, H, roughness);   
        float G   = GeometrySmith(normal, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; 
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(normal, L), 0.0);        

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   

    // Spot Lights
    for(int i = 0; i < numSpotLights; i++) 
    {
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
        float G   = GeometrySmith(normal, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, L), 0.0) + 0.0001; 
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(normal, L), 0.0);        

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    
    // ambient lighting
    vec3 ambient = vec3(0.03) * ao * albedo;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = UnchartedTonemap(color);
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}