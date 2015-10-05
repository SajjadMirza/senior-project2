#version 330 core

in vec3 vertPos;
in vec3 vertNor;
in vec2 vertTex;

out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;
out vec3 lightPos;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 uLightPos;


void main()
{
    vec4 worldPos = M * vec4(vertPos, 1.0f);
//2    worldPos = worldPos - vec4(0, 0, 0, 0);
    fragPos = (V * worldPos).xyz;
    //fragPos = vec3((V * worldPos).xy, worldPos.z);
    //fragPos = worldPos.xyz;
    gl_Position = P * V * worldPos;
    fragTex = vertTex;

    //mat3 normalMatrix = transpose(inverse(mat3(M)));
    mat3 normalMatrix = mat3(M);
    fragNor = normalMatrix * vertNor;

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
