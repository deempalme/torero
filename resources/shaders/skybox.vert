#version 420 core
// GLSL vertex shader for skybox cubemap

layout(location = 0) in vec3 i_position;

out vec3 o_texture;

// projection * static view
uniform mat4 u_pv;

void main()
{
  o_texture = i_position;
  gl_Position =  vec4(u_pv * vec4(i_position, 1.0)).xyww;
//  gl_Position =  vec4(u_pv * vec4(i_position, 1.0));
}
