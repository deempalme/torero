#version 420
// GLSL fragment shader with physically based rendering and fog

in vec3 o_texture;
in vec3 o_up;
in vec3 o_right;

out vec4 o_frag_color;

uniform samplerCube u_cubemap;

const float PI = 3.14159265359;

void main()
{
  // The world vector acts as the normal of a tangent surface
  // from the origin, aligned to o_texture. Given this normal, calculate all
  // incoming radiance of the environment. The result of this radiance
  // is the radiance of light coming from -Normal direction, which is what
  // we use in the PBR shader to sample irradiance.

  vec3 irradiance = vec3(0.0);

  float sampleDelta = 0.025;
  float nrSamples = 0.0;
  for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
      // spherical to cartesian (in tangent space)
      vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
      // tangent space to world
      vec3 sampleVec = tangentSample.x * o_right + tangentSample.y * o_up + tangentSample.z * o_texture;

      irradiance += texture(u_cubemap, sampleVec).rgb * cos(theta) * sin(theta);
      nrSamples++;
    }
  }
  irradiance = PI * irradiance * (1.0 / float(nrSamples));

  o_frag_color = vec4(irradiance, 1.0);
}
