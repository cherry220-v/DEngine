#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
//layout (location = 3) in ivec4 boneIDs;
//layout (location = 4) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 bones[100];

const int MAX_LIGHTS = 8;
uniform mat4 lightSpaceMatrices[MAX_LIGHTS];

//mat4 skinMatrix =
//    bones[boneIDs.x] * weights.x +
//    bones[boneIDs.y] * weights.y +
//    bones[boneIDs.z] * weights.z +
//    bones[boneIDs.w] * weights.w;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 TexCoord;
out vec4 FragPosLightSpace[MAX_LIGHTS];

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);

    FragPos = worldPos.xyz;
    FragNormal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        FragPosLightSpace[i] = lightSpaceMatrices[i] * worldPos;
    }

    gl_Position = projection * view * worldPos;
    //gl_Position = projection * view * skinMatrix * worldPos;
}