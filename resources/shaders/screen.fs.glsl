#version 330 core

in vec2 fragTex;

uniform sampler2D texture0;

out vec4 out_color;

void main()
{
    out_color = vec4(texture2D(texture0, fragTex.st).rgb, 1.0);
}