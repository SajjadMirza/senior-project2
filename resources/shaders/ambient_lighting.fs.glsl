#version 330 core

out vec4 out_color;

in vec2 fragTex;

uniform sampler2D gDiffuse;
uniform float intensity;

void main() 
{
    vec3 fragCol = texture(gDiffuse, fragTex).rgb;
    out_color = vec4(fragCol * intensity, 1.0);
//    out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
