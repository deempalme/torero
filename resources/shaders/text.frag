#version 420 core
// 3D text fragment shader

in vec4 f_color;
//in vec3 f_position;
//in vec2 f_uv;

out vec4 frag_color;

//uniform int u_foggy;

//uniform sampler2D u_font_atlas;

//uniform float u_width;
//uniform float u_edge;

//uniform int u_border;
//uniform vec4 u_border_color;
//uniform vec4 u_border_width_soft_offset;

//uniform vec2 u_offset;

//const float width = 0.5;
//const float edge = 0.1;

//const float border_width = 0.5;
//const float border_edge = 0.5;

//const vec2 offset = vec2(0.0, 0.0);

//const vec4 outline_color = vec4(0.0, 1.0, 0.0, 1.0);

//float calcule_fog(vec3 position, float alpha){
//  const float density = 0.02;
//  const float gradient = 4.0;

//  float distance = length(position);
//  float visibility = exp(-pow((distance * density), gradient));
//  visibility = clamp(visibility, 0.0, 1.0);

//  return alpha * visibility;
//}

void main()
{
//  float distance = 1.0 - texture(u_font_atlas, f_uv).a;
//  float alpha = 1.0 - smoothstep(width, width + edge, distance);

//  float distance2 = 1.0 - texture(u_font_atlas, f_uv + offset).a;
//  float outline_alpha = 1.0 - smoothstep(border_width, border_width + border_edge, distance2);

//  float overall_alpha = alpha + (1.0 - alpha) * outline_alpha;
//  vec3 overall_color = mix(outline_color.xyz, f_color.xyz, alpha / overall_alpha);

//  frag_color.xyz = overall_color;
//  frag_color.a = mix(overall_alpha, calcule_fog(f_position, frag_color.a), u_foggy);

  frag_color = f_color;
}
