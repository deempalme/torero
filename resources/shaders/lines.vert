#version 420 core
// Ground lines vertex shader

layout(location = 0) in vec3 i_position;

out vec3 f_position;
out vec4 f_color;

layout (std140, binding = 1) uniform u_scene {
  mat4 u_projection;
  mat4 u_view;
  mat4 u_pv;
  mat4 u_psv;
  vec3 u_camera_position;
};
uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;
uniform vec4 u_color;

void main()
{
  vec4 position_relative_to_camera = u_view * u_primary_model *
                                     u_secondary_model * vec4(i_position.x, i_position.y - 0.002,
                                                              i_position.z, 1.0);
  f_color = u_color * 0.003921569; // 0.003921569 ~= 1/255
  f_position = position_relative_to_camera.xyz;

  gl_Position = u_projection * position_relative_to_camera;
}
