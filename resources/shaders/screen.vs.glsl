#version 330 core

in vec3 vertPos;
in vec2 vertTex;

in vec4 wordCoords;
uniform int uTextToggle;

out vec2 fragTex;

void main()
{
    if (uTextToggle == 0) {
        gl_Position = vec4(vertPos, 1.0);
        fragTex = vertTex;
    }
    else {
        gl_Position = vec4(wordCoords.xy, 0, 1);
        fragTex = wordCoords.zw;
    }
}
