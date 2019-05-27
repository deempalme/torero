#version 420 core
// Covariance visualizer vertex shader

in vec2 i_position;

in vec3 i_mean;
in mat2 i_inverse_covariance;
in float i_equation_constant;

out vec4 f_position;
out vec4 f_color;

layout (std140) uniform u_scene {
  mat4 u_projection;
  mat4 u_view;
  mat4 u_pv;
};
uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;


void main()
{
  // Positioning
  vec2 multiplication = i_inverse_covariance * i_position;
  float z = exp( -0.5 * (i_position.x * multiplication.x + i_position.y * multiplication.y)
                ) * i_equation_constant;

  f_position = u_primary_model * u_secondary_model *
               vec4(i_mean.xy + i_position, i_mean.z + z, 1.0);
  gl_Position = u_scene.u_pv * f_position;

  // Coloring
  mat4 palette = mat4(0.0, 0.19215686, 1.0, 1.0,
                      0.6470588, 0.752941176, 0.784313725, 0.376470588,
                      0.83529412, 0.745098039, 0.384313725, 0.517647059,
                      1.0, 1.0, 1.0, 1.0);

  int i1 = 0, i2 = 0;
  float between = 0.0;

  z = z * 3.0; // palette.length() - 1;
  i1 = int(floor(z));
  i2 = i1 + 1;
  between = z - float(i1);

  f_color.xyz = (palette[i2] - palette[i1]) * between + palette[i1];
  f_color.a = 1.0;
}
