#version 420 core
//GUI Mouse event fragment shader

in vec2 o_texture;
in float o_id;
in float o_element;

out vec4 frag_color;

void main()
{
  float red = clamp(o_id, 0.0, 1.0);
  float green = clamp(o_id - 1.0, 0.0, 1.0);
  float blue = o_element;

  frag_color = vec4(red, green, blue, 1.0);
}
