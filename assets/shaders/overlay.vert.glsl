#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in uint inColor;
layout(location = 3) in float inDepth;

layout(push_constant) uniform PushConstants
{
    vec2 viewportSize;
};

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

vec4 unpackColor(uint c)
{
    return vec4(
        float((c >> 0)  & 255u),
        float((c >> 8)  & 255u),
        float((c >> 16) & 255u),
        float((c >> 24) & 255u)
    ) / 255.0;
}

void main()
{
    vec2 ndc;

    ndc.x = (inPos.x / viewportSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (inPos.y / viewportSize.y) * 2.0;

    gl_Position = vec4(ndc, inDepth, 1.0);

    fragUV = inUV;
    fragColor = unpackColor(inColor);
}