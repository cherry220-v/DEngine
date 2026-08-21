#version 330 core

in vec2 TexCoord;
in float HeightFactor;

out vec4 FragColor;

uniform sampler2D fireTexture;

uniform float time;

uniform vec4 startColor;
uniform vec4 endColor;

uniform float life;

float noise(vec2 p)
{
    return fract(
        sin(dot(p, vec2(12.9898, 78.233)))
        * 43758.5453
    );
}

void main()
{
    vec2 uv = TexCoord;

    uv.x +=
        sin(uv.y * 10.0 + time * 8.0)
        * 0.03;

    vec4 tex = texture(fireTexture, uv);

    float n =
        noise(uv * 5.0 + time);

    vec4 fireColor =
        mix(endColor, startColor, life);

    fireColor.rgb *=
        1.0 + n * 0.5;

    fireColor.rgb *=
        1.0 + HeightFactor * 0.5;

    float alpha =
        tex.a *
        fireColor.a;

    if (alpha < 0.01)
        discard;

    FragColor =
        vec4(
            tex.rgb * fireColor.rgb,
            alpha
        );
}