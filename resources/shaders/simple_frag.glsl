#version 450

in vec3 fragNor;

out vec4 out_color;

void main()
{
	//vec3 normal = normalize(fragNor);
	vec3 normal = fragNor;
	// Map normal in the range [-1, 1] to color in range [0, 1];
	//vec3 color = 0.5*normal + 0.5;
	vec3 color = normal;
	out_color = vec4(color, 1.0);
}
