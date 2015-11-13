#version 330 core
layout (location = 0) in vec3 vertPos;

in mat4 iM;
uniform mat4 M;
uniform int uInstanced;

void main()
{
    mat4 modelMatrix = uInstanced != 0 ? iM : M;
    gl_Position = modelMatrix * vec4(vertPos, 1.0);
}  
