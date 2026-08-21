#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform bool useTexture;

uniform sampler2D texture1;

uniform vec4 particleColor;

uniform float life;

void main()
{
    vec4 texColor = vec4(1.0);

    if (useTexture)
    {
        texColor =
            texture(texture1, TexCoord);
    }

    vec4 finalColor =
        texColor * particleColor;

    if (finalColor.a < 0.01)
        discard;

    FragColor = finalColor;
}