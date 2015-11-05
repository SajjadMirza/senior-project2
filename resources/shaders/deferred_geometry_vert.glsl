#version 330 core

in vec3 vertPos;
in vec3 vertNor;
in vec2 vertTex;
in vec3 tangent;
in vec3 bitangent;
in mat4 iM;

out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;
out vec3 lightPos;
out mat3 TBN;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 uLightPos;
uniform int uCalcTBN;
uniform int uInstanced;

void main()
{
    mat4 modelMatrix = uInstanced ? iM : M;
    vec4 worldPos = modelMatrix * vec4(vertPos, 1.0f);
    
//2    worldPos = worldPos - vec4(0, 0, 0, 0);
    fragPos = (worldPos).xyz;
    //fragPos = vec3((V * worldPos).xy, worldPos.z);
    //fragPos = worldPos.xyz;
    gl_Position = P * V * worldPos;
    fragTex = vertTex;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    //mat3 normalMatrix = mat3(M);


    if (uCalcTBN == 0) {
        fragNor = normalMatrix * vertNor;
    }
    else {
        fragNor = vertNor;
        vec3 T = normalize(vec3(normalMatrix * vec3(tangent)));
        vec3 B = normalize(vec3(normalMatrix * vec3(bitangent)));
        vec3 N = normalize(vec3(normalMatrix * vec3(vertNor)));
        TBN = mat3(T, B, N);
    }

    vec4 light = M * vec4(uLightPos, 1.0f);
    lightPos = light.xyz;

// ---------------------------------------------
/*    vec4 worldPos = model * vec4(position, 1.0f);
    FragPos = worldPos.xyz; 
    gl_Position = projection * view * worldPos;
    TexCoords = texCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * normal; */
}
