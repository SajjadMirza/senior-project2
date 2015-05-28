#version 130

in vec3 vertPos;
in vec3 vertNor;
in vec2 vertTex;
in vec4 wordCoords;
uniform mat4 P;
uniform mat4 MV;
uniform vec3 uLightPos;
uniform int uTextToggle;
out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;
out vec3 lightPos;

void main()
{
	if (uTextToggle == 0) {
	    vec4 light = MV * vec4(uLightPos, 1.0);
	    lightPos = light.xyz;
	    vec4 posCam = MV * vec4(vertPos, 1.0f);
	    gl_Position = P * posCam;
	    fragPos = posCam.xyz;
	    fragNor = (MV * vec4(vertNor, 0.0)).xyz;
	    fragTex = vertTex;
	}
	else {
		gl_Position = vec4(wordCoords.xy, 0, 1);
		fragTex = wordCoords.zw;
	}
}