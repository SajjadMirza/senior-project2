#version 330 core


layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out float gSpecular;
layout (location = 4) out vec4 gViewSpacePositionDepth;
layout (location = 5) out vec3 gViewSpaceNormal;

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTex;
in mat3 TBN;

uniform sampler2D texture0;
uniform sampler2D texture_norm;
uniform sampler2D texture_spec;
uniform mat4 M;
uniform mat4 V;

uniform int uNormFlag;
uniform int uCalcTBN;
uniform float NEAR;
uniform float FAR;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}

uniform int uHighlight;

void main()
{  
    gViewSpacePositionDepth = (V * vec4(fragPos, 1.0));
    gViewSpacePositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gPosition = fragPos;
    mat4 normalMatrix = transpose(inverse((M)));
    mat3 normalMatrixViewSpace = transpose(inverse(mat3(V * M)));

    if (uNormFlag == 1) {
        gNormal = (vec4(texture2D(texture_norm, fragTex.st).rgb, 0.0)).xyz;
    }
    else {
        gNormal = (fragNor);
        float e = 0.1;
/*
        if (gNormal.x <= e && gNormal.y <= e && gNormal.z <= e &&
            gNormal.x >= -e && gNormal.y >= -e && gNormal.z >= -e) {
            gNormal.xyz = vec3(0, 0.5, 0.5);
        }
*/
//        gNormal = abs(gNormal);
    }

//    gViewSpaceNormal = normalize((normalMatrixViewSpace) * gNormal);

    if (uCalcTBN != 0) {
        gNormal = normalize(gNormal * 2.0 - 1.0);
        gNormal = normalize(TBN * gNormal);
    }

    gViewSpaceNormal = normalize((normalMatrixViewSpace) * gNormal);

//    gDiffuse.rgb = vec3(0.0f, 1.0f, 0.0f);
    gDiffuse.rgb = texture2D(texture0, fragTex).rgb;
    if (uHighlight == 1) {
        gDiffuse.rgb = vec3(1.0, -1.0, 1.0);
    }
    gSpecular = texture2D(texture_spec, fragTex).r;
    // gSpecular = 0.0;

// ----------------------------
/*
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r; */
}
