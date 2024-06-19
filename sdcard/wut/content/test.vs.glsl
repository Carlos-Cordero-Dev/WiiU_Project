
#version 400 core

layout(location = 0) in vec4 aColor;
layout(location = 1) in vec4 aPosition;

out vec4 vColor;

void main()
{
    gl_Position = vec4(aPosition);
    vColor = aColor;
}