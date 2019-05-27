#version 420 core
// GLSL vertex shader for skybox cubemap

layout(location = 0) in vec3 i_position;

out vec3 f_position;

layout (std140, binding = 1) uniform u_scene {
  mat4 u_projection;
  mat4 u_view;
  mat4 u_pv;
  mat4 u_psv;
  vec3 u_camera_position;
};

void main()
{
  f_position = i_position;
  gl_Position = vec4(u_psv * vec4(i_position, 1.0)).xyww;
}
