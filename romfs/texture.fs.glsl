
#version 450

uniform sampler2D tex;

in vec2 vTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture(tex, vTexCoord);
}