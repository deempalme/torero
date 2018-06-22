#version 420 core
// Ground vertex shader

in vec3 i_position;
in vec4 i_color;
in vec2 i_dimension;
in float i_height;
in float i_probability;

out vec4 g_color;
out vec2 g_dimension;
out float g_height;

uniform int u_polar;

uniform int u_grid;
uniform int u_height;
uniform float u_max_height;

void main()
{
  g_dimension = i_dimension;

  if(u_grid == 1){
    float probability = 1.0f / (1.0f + exp(i_probability));
    g_color = vec4(probability, probability, probability, 1.0);
    if(u_height == 1)
      if(probability > 0.0)
        g_height = u_max_height * probability;
      else
        g_height = -u_max_height * 0.2;
    else
      g_height = 0.0;
  }else{
    g_color = i_color/255.0;
    g_height = i_height;
  }

  vec3 position = i_position;

  if(u_polar == 1)
    position = vec3(i_position.x * cos(i_position.y), i_position.x * sin(i_position.y), 0.0);

  gl_Position = vec4(position.x, position.z, position.y, 1.0);
}
