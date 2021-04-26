#version 420 core
// Ground lines fragment shader

in vec4 f_color;
in vec3 f_position;

out vec4 frag_color;

uniform int u_fog;

float calcule_fog(vec3 position, float alpha){
  const float density = 0.02;
  const float gradient = 4.0;

  float distance = length(position);
  float visibility = exp(-pow((distance * density), gradient));
  visibility = clamp(visibility, 0.0, 1.0);

  return alpha * visibility;
}

void main()
{
  frag_color = f_color;
  frag_color.a = mix(frag_color.a, calcule_fog(f_position, frag_color.a), u_fog);
}
