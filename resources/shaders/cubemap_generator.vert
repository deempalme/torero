#version 420 core
// GLSL vertex shader to create a cubemap from an equirectangular map

layout(location = 0) in vec3 i_position;

out vec3 f_position;

uniform mat4 u_pv;

void main()
{
  f_position = normalize(i_position);
  gl_Position =  u_pv * vec4(i_position, 1.0);
}
