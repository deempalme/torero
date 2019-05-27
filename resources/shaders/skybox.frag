#version 420 core
// GLSL fragment shader for skybox cubemap

in vec3 f_position;

layout (location = 0) out vec4 o_frag_color;

uniform samplerCube u_skybox;

void main()
{
  vec3 color = textureLod(u_skybox, f_position, 0.0).rgb;

  // HDR tonemap and gamma correct
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));

  o_frag_color = vec4(color, 1.0);
}
