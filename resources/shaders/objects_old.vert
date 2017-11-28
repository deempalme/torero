#version 420 core
// Obstacles vertex shader

in vec3 i_position;
in vec4 i_color;
in vec3 i_a_s_l;
in vec2 i_lw_t;
in mat4 i_model;
in mat4 i_arrow_model;

out VS_OUT {
  vec4 o_a_s_l_t;
  vec4 o_color;
  mat4 o_model;
  mat4 o_arrow_model;
} vs_out;

uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;

void main()
{
  vs_out.o_a_s_l_t.xyz = i_a_s_l;
  vs_out.o_a_s_l_t.w = i_lw_t.y;
  vs_out.o_color = i_color / 255.0;
  vs_out.o_model = i_model;
  vs_out.o_arrow_model = i_arrow_model;

  gl_Position = u_primary_model * u_secondary_model * vec4(i_position, 1.0f);
  gl_PointSize = i_lw_t.x;
}
