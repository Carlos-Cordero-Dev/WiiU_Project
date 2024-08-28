
#version 450

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPos);
}