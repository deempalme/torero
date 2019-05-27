#version 420 core
// GLSL fragment shader to create a cubemap from an equirectangular map

in vec3 f_position;

layout (location = 0) out vec4 o_frag_color;

uniform sampler2D u_map;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 sample_spherical_map(vec3 v){
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= inv_atan;
  uv += 0.5;
  return uv;
}

void main()
{
  vec2 uv = sample_spherical_map(f_position);
  o_frag_color = vec4(texture(u_map, uv).rgb, 1.0);
}
