#version 130

in vec3 fragPos; // in camera space
in vec3 fragNor; // in camera space
in vec2 fragTex;
uniform sampler2D texture0;
uniform int  mode;
uniform vec3 color;

out vec4 out_color;

void main()
{
    int texture_mode = 116;
    int color_mode = 99;
    vec3 lightPos = vec3(0.0, 2.0, 0.0); // in camera space
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

    /*
      switch(mode) {
      case 't':
      tex = texture2D(texture0, fragTex.st).rgb;
      break;
      case 'c':
      tex = color;
      break;
      default:
      tex = vec3(0.0, 0.0, 1.0);
      }
    */

    vec3 n = normalize(fragNor);
    vec3 l = normalize(lightPos - fragPos);
    vec3 v = -normalize(fragPos);
    vec3 h = normalize(l + v);
    vec3 colorD = max(dot(l, n), 0.0) * tex;
    vec3 colorS = pow(max(dot(h, n), 0.0), 200) * tex;
    vec3 color = colorD + colorS;
    out_color = vec4(color, 1.0);
}
