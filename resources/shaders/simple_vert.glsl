#version 450

in vec3 vertPos;
in vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;
uniform vec3 uColor;
out vec3 fragNor;

void main()
{
	gl_Position = P * MV * vec4(vertPos, 1.0f);
	
	vec4 lightPos = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vec4 normal = MV * normalize(vec4(vertNor, 1.0f));
	
	fragNor = (vec4(uColor, 0.0)).xyz;
    fragNor.x = uColor.x * max(dot(normal, normalize(lightPos)), 0.0f) + (uColor.x/5.0f);
    fragNor.y = uColor.y * max(dot(normal, normalize(lightPos)), 0.0f) + (uColor.y/5.0f);
    fragNor.z = uColor.z * max(dot(normal, normalize(lightPos)), 0.0f) + (uColor.z/5.0f);
}
