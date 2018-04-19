#version 420 core
//GUI 2D fragment shader

in vec2 o_texture;

uniform sampler2D u_albedo;

out vec4 frag_color;

void main()
{
  frag_color = texture(u_albedo, o_texture);
}
