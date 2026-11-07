#version 450 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;
layout(location = 3) in float inDepth;

// inPos is in pixel coordinates with top-left origin (the overlay DrawList
// convention), inColor is a normalized rgba quadruple unpacked from u8.
uniform vec2 viewportSize;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

void main()
{
    vec2 ndc;

    ndc.x = (inPos.x / viewportSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (inPos.y / viewportSize.y) * 2.0;

    gl_Position = vec4(ndc, inDepth, 1.0);

    fragUV = inUV;
    fragColor = inColor;
}
