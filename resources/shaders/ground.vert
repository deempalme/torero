#version 420 core
// Ground vertex shader

in vec2 i_position;
in vec4 i_color;
in float i_height;

out vec4 g_color;
out float g_height;

void main()
{
  g_color = i_color/255.0;
  g_height = i_height;

  gl_Position = vec4(i_position.x, 0.0, i_position.y, 1.0);
}
