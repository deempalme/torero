#version 420 core
// GLSL vertex shader for cubemaps

layout(location = 0) in vec3 i_position;

out vec3 f_position;
out vec3 f_up;
out vec3 f_right;

uniform mat4 u_pv;

void main()
{
  f_position = normalize(i_position);

  // tangent space calculation from origin point
  f_up    = vec3(0.0, 1.0, 0.0);
  f_right = cross(f_up, f_position);
  f_up    = cross(f_position, f_right);

  gl_Position =  u_pv * vec4(i_position, 1.0);
}
