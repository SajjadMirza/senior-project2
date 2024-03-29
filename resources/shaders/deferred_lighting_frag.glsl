#version 330 core

in vec2 fragTex;

struct Light {
    vec3 position;
    vec3 color;
    vec3 specular;
    vec3 ambient;
    float quadratic;
    float linear;
    float intensity;
};
uniform Light light;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform sampler2D occlusion;
uniform samplerCube depthMap;


uniform int uDrawMode;
const int DISPLAY_POSITION = 0;
const int DISPLAY_NORMALS = 1;
const int DISPLAY_COLOR = 2;
const int DISPLAY_DIFFUSE = 3;
const int DISPLAY_SPECULAR = 4;
const int DISPLAY_SHADING = 5;
const int DISPLAY_SPECULAR_BUFFER = 6;
const int DISPLAY_SHADOW_DEPTH = 7;
const int DISPLAY_SHADOW_BIAS = 8;

uniform vec3 viewPos;
uniform vec2 uScreenSize;
uniform float far_plane;

out vec4 out_color;

uniform int uTextToggle;
uniform int uShadow;

float debug_color_depth;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float calc_shadow(vec3 fragPos, float bias)
{
    vec3 directionVector = fragPos - light.position;
    float currentDepth = length(directionVector);
    int samples = 20;

    float shadow = 0.0;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    debug_color_depth = texture(depthMap, directionVector).r;

    for (int i = 0; i < samples; ++i) {
        float nearDepth = texture(depthMap, directionVector + sampleOffsetDirections[i] * diskRadius).r;
        nearDepth *= far_plane;
        if (currentDepth - bias > nearDepth) {
            shadow += 1.0;
        }
    }
    
    shadow /= float(samples);


/*
    float nearDepth = texture(depthMap, directionVector).r;
    shadow = currentDepth - bias > nearDepth ? 1.0 : 0.0;
*/    
    return shadow;
}

void main()
{
    if (uTextToggle == 0) {
       
  
        
        vec2 texCoord = gl_FragCoord.xy / uScreenSize;
//        float fragOcc = texture(occlusion, texCoord).r;
        float fragOcc = 1.0;
        vec3 fragPos = texture(gPosition, texCoord).rgb;
        float dist = length(light.position - fragPos);
        float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
        
        vec3 fragNor = (texture(gNormal, texCoord).rgb);
        vec3 fragCol = texture(gDiffuse, texCoord).rgb;
        float spc = texture(gSpecular, texCoord).r;
        vec3 fragSpc = vec3(spc, spc, spc);

//        vec3 ambient = fragCol * 0.1 * light.ambient;
        vec3 viewDir = normalize(viewPos - fragPos);
        
        vec3 lightDir = normalize(light.position - fragPos);
        vec3 diffuse = max(dot(fragNor, lightDir), 0.0) * fragCol * light.color * light.intensity;
        vec3 halfDir = normalize(lightDir + viewDir);
        vec3 reflectDir = reflect(-lightDir, fragNor);
        float spec = pow(max(dot(fragNor, halfDir), 0.0), 64.0);
        vec3 specular = light.specular * spec * fragSpc * light.intensity;

        float bias;
//        bias = max(0.1 * (1.0 - dot(fragNor, lightDir)), 0.005);  
        bias = 0.15;
//        bias = 0.0;
        float shadow = 0.0;
        if (uShadow == 1) {
            shadow = calc_shadow(fragPos, bias);
        }
        else {
            debug_color_depth = 0.0;
        }
        
        diffuse *= attenuation;
        specular *= attenuation;
//        vec3 light = diffuse + specular + ambient;

//        vec3 light = (diffuse + specular * 0.5) * (1.0 - shadow) * 2;

        vec3 light = (diffuse * 1.0 + specular * 1.0) * (1.0 - shadow);
        light *= fragOcc;
        light *= 1.5;

//        vec3 light = (diffuse * 2.0) * (1.0 - shadow);        

        vec3 data = vec3(1.0, 0.0, 1.0);
        switch (uDrawMode) {
        case DISPLAY_POSITION:
            data = fragPos;
            break;
        case DISPLAY_NORMALS:
            data = (fragNor);
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
        case DISPLAY_SHADOW_DEPTH:
            data = vec3(debug_color_depth);
            break;
        case DISPLAY_SHADOW_BIAS:
            data = vec3(fragOcc);
            break;
        }
        
//        data = vec3(fragOcc, fragOcc, fragOcc);
//        data = vec3(1.0) - data;
//        data = vec3(1.0, 1.0, 1.0);
        out_color = vec4(data, 1.0);
//        out_color = vec4(1.0, 0.0, 1.0, 1.0);
//        out_color = vec4(vec3(debug_color), 1.0);
    }
    else {
        out_color = vec4(0.0, 1.0, 1.0, texture2D(gPosition, fragTex.st).a);
    }

}
