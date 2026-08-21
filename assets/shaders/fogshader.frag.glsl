#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform vec3 fogColor;

uniform float fogDensity;

uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;

    return
        (2.0 * nearPlane * farPlane) /
        (farPlane + nearPlane -
        z * (farPlane - nearPlane));
}

void main()
{
    vec4 scene =
        texture(sceneTexture, TexCoord);

    float depth =
        texture(depthTexture, TexCoord).r;

    float linearDepth =
        linearizeDepth(depth);

    float fogFactor =
        exp(-fogDensity * linearDepth);

    fogFactor =
        clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor =
        mix(
            fogColor,
            scene.rgb,
            fogFactor
        );

    FragColor =
        vec4(finalColor, scene.a);
}