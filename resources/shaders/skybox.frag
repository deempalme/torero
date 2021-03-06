#version 420 core
// GLSL fragment shader for skybox cubemap

in vec3 o_texture;

out vec4 o_frag_color;

uniform samplerCube u_skybox;

void main()
{
//  vec3 color = texture(u_skybox, o_texture).rgb;

  // HDR tonemap and gamma correct
//  color = color / (color + vec3(1.0));
//  color = pow(color, vec3(1.0/2.2));

  o_frag_color = vec4(texture(u_skybox, o_texture).rgb, 1.0);
//  o_frag_color = vec4(color, 1.0);
}
