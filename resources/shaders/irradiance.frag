#version 420 core
// GLSL fragment shader for irradiance convolution

in vec3 o_position;
in vec3 o_up;
in vec3 o_right;

out vec4 frag_color;

uniform samplerCube u_skybox;

const float PI = 3.14159265359;

void main()
{		
  vec3 irradiance = vec3(0.0);

  float sampleDelta = 0.025;
  float nrSamples = 0.0f;

  float sin_theta, sin_phi, cos_theta, cos_phi;

  for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
    sin_phi = sin(phi);
    cos_phi = cos(phi);

    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
      sin_theta = sin(theta);
      cos_theta = cos(theta);

      // spherical to cartesian (in tangent space)
      vec3 tangentSample = vec3(sin_theta * cos_phi,  sin_theta * sin_phi, cos_theta);
      // tangent space to world
      vec3 sampleVec = tangentSample.x * o_right + tangentSample.y * o_up
          + tangentSample.z * o_position;

      irradiance += texture(u_skybox, sampleVec).rgb * cos_theta * sin_theta;
      nrSamples++;
    }
  }
  irradiance = (PI * irradiance) / float(nrSamples);

  frag_color = vec4(irradiance, 1.0);
}
