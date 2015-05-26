#version 130

in vec3 vertPos;

uniform mat4 P;
uniform mat4 MV;
uniform vec3 uColor;

out vec3 fragPos;

void main()
{
    vec4 posCam = MV * vec4(vertPos, 1.0f);
    gl_Position = P * posCam;
    fragPos = posCam.xyz;


}