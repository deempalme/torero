#version 420 core
// GUI Text vertex shader

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_uv;
layout(location = 2) in vec2 i_size;
layout(location = 5) in vec4 i_color;

out vec2 g_size;
out vec4 g_color;
out vec4 g_texture;

void main()
{
  g_size = i_size;
  g_color = i_color;
  g_texture = i_uv;

  gl_Position = vec4(i_position, 1.0);
}
