#version 130

in vec3 fragPos; // in camera space
in vec3 fragNor; // in camera space
in vec2 fragTex;
in vec3 lightPos;
uniform sampler2D texture0;
uniform int  mode;
uniform vec3 color;
uniform int uTextToggle;
uniform int uRedder;
out vec4 out_color;

void main()
{
    if (uTextToggle == 0) {
        int texture_mode = 116;
        int color_mode = 99;
        vec3 tex;

        if (mode == texture_mode) {
            tex = texture2D(texture0, fragTex.st).rgb;
        }
        else if (mode == color_mode) {
            tex = color;
        }
        else {
            tex = vec3(0.0, 0.0, 1.0);
        }

        vec3 n = normalize(fragNor);
        vec3 l = normalize(lightPos - fragPos);
        vec3 v = -normalize(fragPos);
        vec3 h = normalize(l + v);
        vec3 colorD = max(dot(l, n), 0.0) * tex;
        vec3 colorS = pow(max(dot(h, n), 0.0), 200) * tex;
        vec3 colorA = tex * 0.2f;
        if (uRedder == 1) {
            colorA = vec3(0.3, 0, 0);
        }

        vec3 col = colorD + colorS + colorA;
        out_color = vec4(col, 1.0);
  }
  else {
    out_color = vec4(1.0, 0.0, 0.0, texture2D(texture0, fragTex.st).a);
  }
}
