#version 330 core

out float ssao_out;

in vec2 fragTex;

uniform sampler2D gViewSpacePositionDepth;
uniform sampler2D gNormal;
uniform sampler2D ssaoNoise;
uniform int screen_width;
uniform int screen_height;

uniform vec3 samples[64];

int kernelSize = 16;
float radius = 1.0;

vec2 noiseScale = vec2(float(screen_width) / 4.0f, float(screen_height) / 4.0f);

uniform mat4 V;
uniform mat4 P;


void main()
{
    vec3 fragPos = texture(gViewSpacePositionDepth, fragTex).xyz;
    vec3 fragNor = texture(gNormal, fragTex).rgb;
//    fragNor = (transpose(vec4(fragNor, 1.0)) * V).xyz
//    fragNor = (V * vec4(fragNor, 1.0)).xyz;
    vec3 randomVec = texture(ssaoNoise, fragTex * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - fragNor * dot(randomVec, fragNor));
//    vec3 tangent = normalize(fragNor);
    vec3 bitangent = cross(fragNor, tangent);
    mat3 TBN = mat3(tangent, bitangent, fragNor);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; i++) {
        vec3 samplevec = TBN * samples[i];
        samplevec = fragPos + samplevec * radius;

        vec4 offset = vec4(samplevec, 1.0);
        offset = P * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth = -texture(gViewSpacePositionDepth, offset.xy).w;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth ));
        occlusion += (sampleDepth >= samplevec.z ? 1.0 : 0.0) * rangeCheck;
//        occlusion += (sampleDepth >= samplevec.z ? 1.0 : 0.0);
    }
    
    occlusion = 1.0 - (occlusion / kernelSize);

    ssao_out = occlusion;
//    ssao_out = 0.5;
}
