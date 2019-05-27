#version 420 core
//GUI Mouse event vertex shader

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec2 i_size;
layout(location = 3) in float i_id;
layout(location = 4) in float i_element;

out vec2 o_texture;
out float o_id;
out float o_element;

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

  o_texture = i_uv / u_texture_size;
  o_id = i_id/255.0;
  o_element = i_element/255.0;

  gl_Position = vec4(position.yx, 0.0, 1.0);
}
