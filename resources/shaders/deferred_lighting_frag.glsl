#version 330 core

in vec2 fragTex;

struct Light {
    vec3 position;
    vec3 color;
    float quadratic;
    float linear;
};
uniform Light light;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

uniform int uDrawMode;
const int DISPLAY_POSITION = 0;
const int DISPLAY_NORMALS = 1;
const int DISPLAY_COLOR = 2;
const int DISPLAY_DIFFUSE = 3;
const int DISPLAY_SPECULAR = 4;
const int DISPLAY_SHADING = 5;
const int DISPLAY_SPECULAR_BUFFER = 6;

uniform vec3 viewPos;

out vec4 out_color;


void main()
{
    vec3 fragPos = texture(gPosition, fragTex).rgb;
    vec3 fragNor = texture(gNormal, fragTex).rgb;
    vec3 fragCol = texture(gDiffuse, fragTex).rgb;
    float spc = texture(gSpecular, fragTex).r;
    vec3 fragSpc = vec3(spc, spc, spc);

    vec3 ambient = fragCol * 0.1;
    vec3 viewDir = normalize(viewPos - fragPos);
    
    float dist = length(light.position - fragPos);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 diffuse = max(dot(fragNor, lightDir), 0.0) * fragCol * light.color;
//    vec3 diffuse = fragCol * light.color;
    vec3 halfDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(fragNor, halfDir), 0.0), 16.0);
    vec3 specular = vec3(1.0, 1.0, 1.0) * spec * fragSpc;
    float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
    diffuse *= attenuation;
//    diffuse * (1.0 / (1.0 + dist));
    specular *= attenuation;
    vec3 light = diffuse + specular + ambient;

    vec3 data = vec3(1.0, 0.0, 1.0);
    switch (uDrawMode) {
    case DISPLAY_POSITION:
        data = fragPos;
        break;
    case DISPLAY_NORMALS:
        data = fragNor;
        break;
    case DISPLAY_COLOR:
        data = fragCol;
        break;
    case DISPLAY_DIFFUSE:
        data = diffuse;
        break;
    case DISPLAY_SPECULAR:
        data = specular;
        break;
    case DISPLAY_SHADING:
        data = light;
        break;
    case DISPLAY_SPECULAR_BUFFER:
        data = fragSpc;
        break;
    }

    out_color = vec4(data, 1.0);
}