#version 420
//point cloud fragment shader

in vec4 o_color;

out vec4 o_frag_color;

void main()
{
  o_frag_color = o_color;
}
