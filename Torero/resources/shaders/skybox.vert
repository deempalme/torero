#version 420
// GLSL vertex shader for skybox cubemap

in vec3 i_position;

out vec3 o_texture;

// projection * static view
uniform mat4 u_pv;

void main()
{
  o_texture = i_position;
  gl_Position =  vec4(u_pv * vec4(o_texture, 1.0)).xyww;
}
