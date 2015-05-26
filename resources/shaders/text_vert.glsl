#version 130

in vec3 vertPos;
in vec3 vertNor;
in vec2 vertTex;
uniform mat4 P;
uniform mat4 MV;
out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;
out vec3 lightPos;

void main()
{
    vec4 light = MV * vec4(0.0, 20.0, 0.0, 1.0);
    lightPos = light.xyz;
    vec4 posCam = MV * vec4(vertPos, 1.0f);
    gl_Position = P * posCam;
    fragPos = posCam.xyz;
    fragNor = (MV * vec4(vertNor, 0.0)).xyz;
    fragTex = vertTex;
}