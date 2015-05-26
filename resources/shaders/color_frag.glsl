#version 130

in vec3 fragPos;

uniform vec3 uColor;

out vec4 out_color;

void main()
{
    out_color = vec4(uColor, 1.0);
}