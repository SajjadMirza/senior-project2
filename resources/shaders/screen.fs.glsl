#version 330 core

in vec2 fragTex;

uniform sampler2D texture0;
uniform int uTextToggle;

out vec4 out_color;

void main()
{
    if (uTextToggle == 0) {
        out_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else {
        out_color = vec4(0.0, 255.0/255.0, 0.0, texture2D(texture0, fragTex.st).a);
    }
}