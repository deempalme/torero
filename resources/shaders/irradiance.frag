#version 420 core
// GLSL fragment shader for irradiance convolution

in vec3 f_position;
in vec3 f_up;
in vec3 f_right;

layout (location = 0) out vec4 frag_color;

uniform samplerCube u_skybox;

const float PI = 3.14159265359;

void main()
{		
  vec3 irradiance = vec3(0.0);

  float sample_delta = 0.025;
  float nr_samples = 0.0f;

  float sin_theta, sin_phi, cos_theta, cos_phi;

  for(float phi = 0.0; phi < 2.0 * PI; phi += sample_delta){
    sin_phi = sin(phi);
    cos_phi = cos(phi);

    for(float theta = 0.0; theta < 0.5 * PI; theta += sample_delta){
      sin_theta = sin(theta);
      cos_theta = cos(theta);

      // spherical to cartesian (in tangent space)
      vec3 tangent_sample = vec3(sin_theta * cos_phi,  sin_theta * sin_phi, cos_theta);
      // tangent space to world
      vec3 sample_vec = tangent_sample.x * f_right + tangent_sample.y * f_up
          + tangent_sample.z * f_position;

      irradiance += texture(u_skybox, sample_vec).rgb * cos_theta * sin_theta;
      ++nr_samples;
    }
  }
  irradiance = (PI * irradiance) / float(nr_samples);

  frag_color = vec4(irradiance, 1.0);
}
