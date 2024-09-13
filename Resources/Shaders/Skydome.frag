#version 460 core
out vec4 FragColor;
in vec3 TexCoords;

uniform sampler2D hdrTexture;

void main()
{
    vec3 normalized = normalize(TexCoords);

    // Handle edge cases by clamping y to avoid invalid texture coordinates
    float theta = atan(normalized.z, normalized.x); // Range [-PI, PI]
    float phi = asin(clamp(normalized.y, -1.0, 1.0)); // Range [-PI/2, PI/2]

    // Convert 3D coordinates to 2D texture coordinates for equirectangular mapping
    vec2 uv = vec2(theta / (2.0 * 3.14159) + 0.5, phi / 3.14159 + 0.5);

    // Sample the HDR texture
    vec3 hdrColor = texture(hdrTexture, uv).rgb;

    FragColor = vec4(hdrColor, 1.0);
}
