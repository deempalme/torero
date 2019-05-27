#version 420 core
// Trajectory vertex shader

layout(location = 0) in vec3 i_position;
layout(location = 1) in float i_distance;
layout(location = 2) in float i_line_width;
layout(location = 4) in float i_angle;
layout(location = 5) in vec4 i_color;

out vec4 g_color;
out float g_line_width;
out float g_distance;
out float g_angle;

uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;

void main()
{
  g_color = i_color;
  g_line_width = i_line_width;
  g_distance = i_distance;
  g_angle = i_angle;

  gl_Position = u_primary_model * u_secondary_model * vec4(i_position.x, i_position.y - 0.001,
                                                           i_position.z, 1.0f);
}
