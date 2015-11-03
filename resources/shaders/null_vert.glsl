#version 330 core

in vec3 vertPos;

uniform float scale;

void main()
{
    vec3 pos = vec3(vertPos.x, vertPos.y, vertPos.z - 0.2);
    gl_Position = vec4(scale * pos, 1.0);
}
