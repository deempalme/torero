#version 420 core
// 3D text fragment shader

in vec4 f_color;
in vec3 f_position;
in vec3 f_uv;

out vec4 frag_color;

uniform int u_fog;

uniform sampler2D u_font_atlas;

uniform float u_width;
uniform float u_edge;

uniform float u_border_width;
uniform float u_border_edge;

uniform vec2 u_offset;
uniform vec3 u_outline_color;

float calcule_fog(vec3 position, float alpha){
  const float density = 0.02;
  const float gradient = 4.0;

  float distance = length(position);
  float visibility = exp(-pow((distance * density), gradient));
  visibility = clamp(visibility, 0.0, 1.0);

  return alpha * visibility;
}

void main()
{
  float distance = 1.0 - texture(u_font_atlas, f_uv).a;
  float alpha = 1.0 - smoothstep(width, width + edge, distance);

  float distance2 = 1.0 - texture(u_font_atlas, f_uv + u_offset).a;
  float outline_alpha = 1.0 - smoothstep(u_border_width, u_border_width + u_border_edge, distance2);

  float overall_alpha = alpha + (1.0 - alpha) * outline_alpha;
  vec3 overall_color = mix(u_outline_color, f_color, alpha / overall_alpha);

  frag_color = vec4(overall_color, overall_alpha);
  frag_color.a = mix(frag_color.a, calcule_fog(f_position, frag_color.a), u_fog);
}
