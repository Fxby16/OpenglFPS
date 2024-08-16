#version 460 core

const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;
layout (location = 5) in int boneIDs[MAX_BONE_INFLUENCE];
layout (location = 6) in int weights[MAX_BONE_INFLUENCE];

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragTangent;
out vec3 fragBinormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate binormal from vertex normal and tangent
    vec3 vertexBinormal = cross(vertexNormal, vertexTangent);

    // Calculate fragment normal based on normal transformations
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    // Calculate fragment position based on model transformations
    fragPosition = vec3(model * vec4(vertexPosition, 1.0f));

    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragNormal = normalize(normalMatrix * vertexNormal);

    fragTangent = normalize(normalMatrix * vertexTangent);
    fragTangent = normalize(fragTangent - dot(fragTangent, fragNormal) * fragNormal);
    fragBinormal = normalize(normalMatrix * vertexBinormal);
    fragBinormal = cross(fragNormal, fragTangent);

    // Calculate final vertex position
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
