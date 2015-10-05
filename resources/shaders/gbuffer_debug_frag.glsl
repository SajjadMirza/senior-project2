#version 330 core

in vec2 fragTex;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;

uniform int gBufferMode;
const int DISPLAY_POSITION = 0;
const int DISPLAY_NORMALS = 1;
const int DISPLAY_DIFFUSE = 2;

out vec4 out_color;

void main()
{
    vec3 data = vec3(1, 0, 0);
    switch (gBufferMode) {
    case DISPLAY_POSITION:
        data = texture(gPosition, fragTex).rgb;
        break;
    case DISPLAY_NORMALS:
        data = texture(gNormal, fragTex).rgb;
        break;
    case DISPLAY_DIFFUSE:
        data = texture(gDiffuse, fragTex).rgb;
        break;
    }
    
//    out_color = vec4(data, 1.0f);
//    out_color = vec4(fragTex, 0.0f, 1.0f);
}
