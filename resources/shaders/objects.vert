#version 420 core
// Obstacles vertex shader

in vec3 i_position;
in vec3 i_normal;
in vec2 i_uv;
in vec3 i_scales;

in vec3 i_translation;
in vec3 i_rotation;
in vec4 i_color;
in vec3 i_scale;
in float i_line_width;

out vec4 f_position;
out vec3 f_normal;
out vec4 f_color;
out vec2 f_uv;

uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;
uniform mat4 u_pv;

// Rotate matrix by pitch, yaw, roll
mat4 rotate_matrix(vec3 angles){
  const float cosA = cos(angles.x);
  const float sinA = sin(angles.x);
  const float cosB = cos(angles.y);
  const float sinB = sin(angles.y);
  const float cosC = cos(angles.z);
  const float sinC = sin(angles.z);

  mat4 r_m =
      mat4( cosC * cosB - sinC * sinA * sinB, -sinC * cosA, cosC * sinB + sinC * sinA * cosB, 0,
            sinC * cosB + cosC * sinA * sinB,  cosC * cosA, sinC * sinB - cosC * sinA * cosB, 0,
           -cosA * sinB,                       sinA,        cosA * cosB,                      0,
            0,                                 0,           0,                                1);

  return r_m;
}

void main()
{
  vec3 scale;
  scale.x = mix(i_scale.x, i_scale.x * 1.25 - i_line_width * 2, i_scales.x);
  scale.y = mix(i_scale.y, i_scale.y * 1.25 - i_line_width * 2, i_scales.y);
  scale.z = mix(i_scale.z, i_scale.z * 1.25 - i_line_width * 2, i_scales.z);

  mat4 rotation = rotate_matrix(i_rotation);

  f_position = u_primary_model * u_secondary_model *
               (rotation * vec4(i_position * scale, 1.0) + vec4(i_translation, 0.0));
  gl_Position = u_pv * f_position;
  f_color = i_color/255.0;
  f_normal = vec4(rotation * vec4(i_normal, 1.0)).xyz;
  f_uv = i_uv;
}
