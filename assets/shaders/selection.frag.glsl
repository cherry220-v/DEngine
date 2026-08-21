#version 330 core

layout(location = 0) out uint FragColor;
uniform uint objectID;

void main()
{
    FragColor = objectID;
}