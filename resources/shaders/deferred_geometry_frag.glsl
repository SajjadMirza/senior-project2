#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuse;

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTex;

uniform sampler2D texture0;
uniform sampler2D texture_norm;

uniform int uNormFlag;

void main()
{    

    gPosition = fragPos;

    if (uNormFlag) {
        gNormal = normalize(M * vec4(texture2D(texture_norm, fragTex.st).rgb, 0.0)).xyz;
    }
    else {
        gNormal = normalize(fragNor);
    }

    gDiffuse.rgb = texture2D(texture0, fragTex).rgb;

// ----------------------------
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}
