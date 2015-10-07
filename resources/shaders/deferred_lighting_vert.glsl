#version 330 core

layout (location = 5) in vec3 vertPos;
layout (location = 6) in vec2 vertTex;

out vec2 fragTex;

void main()
{
    gl_Position = vec4(vertPos, 1.0f);
    fragTex = vertTex;
}
