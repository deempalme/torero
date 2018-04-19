#version 420 core
// GUI Text fragment shader

in vec4 f_color;
in vec2 f_uv;

out vec4 frag_color;

uniform sampler2D u_font_atlas;

const float width = 0.45;
const float edge = 0.1;

void main()
{
  float distance = 1.0 - texture(u_font_atlas, f_uv).a;
  float alpha = 1.0 - smoothstep(width, width + edge, distance);

  frag_color = vec4(f_color.rgb, f_color.a * alpha);
//  frag_color = texture(u_font_atlas, f_uv) * f_color;
}
