#version 420 core
// GLSL vertex shader for bidirectional reflective distribution function

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_uv;

out vec2 o_uv;

void main()
{
  o_uv = i_uv;
  gl_Position = vec4(i_position, 1.0);
}
