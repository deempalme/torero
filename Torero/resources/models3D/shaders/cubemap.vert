#version 420
// GLSL vertex shader for cubemaps

in vec3 i_position;

out vec3 o_texture;
out vec3 o_up;
out vec3 o_right;

// projection * view
uniform mat4 u_pv;

void main()
{
  o_texture = normalize(i_position);

  // tangent space calculation from origin point
  o_up    = vec3(0.0, 1.0, 0.0);
  o_right = cross(o_up, o_texture);
  o_up    = cross(o_texture, o_right);

  gl_Position =  u_pv * vec4(i_position, 1.0);
}
