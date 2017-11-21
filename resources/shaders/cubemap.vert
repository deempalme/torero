#version 420 core
// GLSL vertex shader for cubemaps

layout(location = 0) in vec3 i_position;

out vec3 o_position;
out vec3 o_up;
out vec3 o_right;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
  o_position = normalize(i_position);

  // tangent space calculation from origin point
  o_up    = vec3(0.0, 1.0, 0.0);
  o_right = cross(o_up, o_position);
  o_up    = cross(o_position, o_right);

  gl_Position =  u_projection * u_view * vec4(i_position, 1.0);
}
