#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlurTexture;

uniform float exposure;
uniform int toneMappingType;
uniform float bloomStrength;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec3 Uncharted2Tonemap(vec3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 FilmicTonemap(vec3 x)
{
    float A = 0.22;
    float B = 0.30;
    float C = 0.10;
    float D = 0.20;
    float E = 0.01;
    float F = 0.30;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 ReinhardTonemap(vec3 x)
{
    return x / (x + vec3(1.0));
}

void main()
{
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlurTexture, TexCoords).rgb;
    
    //vec3 result = mix(hdrColor, bloomColor, bloomStrength);
    vec3 result = hdrColor + bloomColor * bloomStrength;

    result *= exposure;

    if (toneMappingType == 0)
        result = ACESFilm(result);
    else if (toneMappingType == 1)
        result = Uncharted2Tonemap(result);
    else if (toneMappingType == 2)
        result = FilmicTonemap(result);
    else if (toneMappingType == 3)
        result = ReinhardTonemap(result);
    else
        result = Uncharted2Tonemap(result);

    result = pow(result, vec3(1.0 / 2.2));           // gamma correction

    FragColor = vec4(result, 1.0);
}