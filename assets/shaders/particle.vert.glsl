#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 particlePos;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform float size;

void main()
{
    vec3 worldPos =
        particlePos +
        cameraRight * aPos.x * size +
        cameraUp * aPos.y * size;

    gl_Position =
        projection *
        view *
        vec4(worldPos, 1.0);

    TexCoord = aUV;
}