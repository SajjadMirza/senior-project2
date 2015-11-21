#version 330 core

in vec3 vertPos;
in vec2 vertTex;

out vec2 fragTex;

void main()
{
    gl_Position = vec4(vertPos, 1.0f);
    fragTex = vertTex;
}
