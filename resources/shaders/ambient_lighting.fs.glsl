#version 330 core

out vec4 out_color;

in vec2 fragTex;

uniform sampler2D gDiffuse;
uniform sampler2D occlusion;
uniform float intensity;

void main() 
{
    // float fragOcc = texture(occlusion, fragTex).r;
    float fragOcc = 1.0;
    vec3 fragCol = texture(gDiffuse, fragTex).rgb;
    fragCol *= fragOcc;
    out_color = vec4(fragCol * intensity, 1.0);
//    out_color = vec4(vec3(fragOcc), 1.0);
//    out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
