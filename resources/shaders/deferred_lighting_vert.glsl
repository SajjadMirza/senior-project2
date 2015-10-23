#version 330 core

layout (location = 5) in vec3 vertPos;
layout (location = 6) in vec2 vertTex;

in vec4 wordCoords;
uniform int uTextToggle;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

out vec2 fragTex;

void main()
{
	if (uTextToggle == 0) {
    	gl_Position = P * V * M * vec4(vertPos, 1.0f);
    	fragTex = vertTex;
	}
	else {
		gl_Position = vec4(wordCoords.xy, 0, 1);
		fragTex = wordCoords.zw;
	}
}
