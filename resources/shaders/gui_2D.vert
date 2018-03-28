#version 420 core
//GUI 2D vertex shader

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_texture;
layout(location = 2) in vec2 i_size;

out vec2 o_texture;

uniform vec2 u_window_size;
uniform vec2 u_texture_size;
uniform vec2 u_offset;
uniform vec2 u_alignment;

void main()
{
  vec2 position = (i_position.y < 0.0)? vec2(i_position.x, u_window_size.x + i_position.y) :
                                        i_position;
  position += u_offset + u_alignment;
  position = (position / u_window_size.yx) * 2.0 - 1.0;

  position.x = -position.x;

  o_texture = i_texture / u_texture_size;

  gl_Position = vec4(position.yx, 0.0, 1.0);
}
