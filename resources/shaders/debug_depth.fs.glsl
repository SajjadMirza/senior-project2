#version 330 core

in vec2 fragTex;

uniform sampler2D depthTexture;
out vec4 out_color;

void main()
{
    float depth = texture(depthTexture, fragTex).r;
    out_color = vec4(depth, depth, depth, 1.0f);
}
