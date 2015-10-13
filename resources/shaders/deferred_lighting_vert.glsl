#version 330 core

layout (location = 5) in vec3 vertPos;
layout (location = 6) in vec2 vertTex;

in vec4 wordCoords;
uniform int uTextToggle;

out vec2 fragTex;

void main()
{
	if (uTextToggle == 0) {
    	gl_Position = vec4(vertPos, 1.0f);
    	fragTex = vertTex;
	}
	else {
		gl_Position = vec4(wordCoords.xy, 0, 1);
		fragTex = wordCoords.zw;
	}
}
