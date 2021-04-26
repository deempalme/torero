#version 420 core
// Ground vertex shader

in vec3 i_position;
in vec4 i_color;
in vec2 i_dimension;
in float i_height;

out vec4 g_color;
out vec2 g_dimension;
out float g_height;

uniform int u_polar;

void main()
{
  g_color = i_color/255.0;
  g_dimension = i_dimension;
  g_height = i_height;

  vec3 position = i_position;

  if(u_polar == 1)
    position = vec3(i_position.x * cos(i_position.y), i_position.x * sin(i_position.y), 0.0);

  gl_Position = vec4(position.x, position.z, position.y, 1.0);
}
