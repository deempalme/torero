#version 420 core
//point cloud fragment shader

// Forcing early fragment test
//layout(early_fragment_tests) in;

in vec4 o_color;

layout (location = 0) out vec4 o_frag_color;

void main()
{
  o_frag_color = o_color;
}
