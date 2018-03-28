#version 420 core
// Text vertex shader

in vec3 i_origin;
//in vec2 i_position;
//in vec2 i_size;
//in vec4 i_color;
//in vec4 i_texture;
//in float i_angle;

//out vec3 g_position;
//out vec2 g_offset;
//out vec3 g_size_angle;
//out vec4 g_color;
//out vec4 g_texture;

uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;
//uniform mat4 u_view;
//uniform mat4 u_projection;

//uniform int u_colored;
//uniform vec4 u_color;

//uniform float u_oriented;

//uniform mat4 u_view;
//uniform mat4 u_projection;

void main()
{
//  g_offset = i_position;
//  g_size_angle = vec3(i_size, mix(0.0, i_angle, u_oriented));
//  g_texture = i_texture;
//  g_color = mix(i_color/255.0, u_color/255.0, u_colored);

//  vec4 position_relative_to_camera = u_view * u_primary_model *
//  vec4 position_relative_to_camera = u_primary_model *
//                                     u_secondary_model * vec4(i_origin, 1.0);

//  g_position = position_relative_to_camera.xyz;

//  gl_Position = u_projection * position_relative_to_camera;
  gl_Position = u_primary_model * u_secondary_model * vec4(i_origin, 1.0);
//  gl_Position = position_relative_to_camera;
}
