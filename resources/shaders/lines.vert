#version 420 core
// Ground lines vertex shader

in vec3 i_position;

out vec4 f_color;

uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec4 u_color;
uniform int u_fog;

float calcule_fog(vec4 position, float alpha){
  const float density = 0.010;
  const float gradient = 3.0;

  float distance = length(position.xyz);
  float visibility = exp(-pow((distance * density), gradient));
  visibility = clamp(visibility, 0.0, 1.0);

  return alpha * visibility;
}

void main()
{
  vec4 position_relative_to_camera = u_view * u_primary_model *
                                     u_secondary_model * vec4(i_position.x, i_position.y - 0.002,
                                                              i_position.z, 1.0);
  gl_Position = u_projection * position_relative_to_camera;
  f_color = u_color/255.0;
  f_color.a = mix(f_color.a, calcule_fog(position_relative_to_camera, f_color.a), u_fog);
}
