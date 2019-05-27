#version 420 core
// Obstacles vertex shader

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_scales;

in vec3 i_translation;
in vec4 i_rotation;
in vec4 i_color;
in vec3 i_scale;
in float i_line_width;

out vec4 f_position;
out vec3 f_normal;
out vec4 f_color;
out vec2 f_uv;

layout (std140, binding = 1) uniform u_scene {
  mat4 u_projection;
  mat4 u_view;
  mat4 u_pv;
  mat4 u_psv;
  vec3 u_camera_position;
};
uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;

// Rotate vector using quaternions
vec3 quaternion_rotation(const vec3 v, const vec4 q){
  if(all(equal(q, vec4(0.0, 0.0, 0.0, 1.0))))
    return v;
  return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

void main()
{
  vec3 scale;
  scale.x = mix(i_scale.x, i_scale.x * 1.25 - i_line_width * 2, i_scales.x);
  scale.y = mix(i_scale.y, i_scale.y * 1.25 - i_line_width * 2, i_scales.y);
  scale.z = mix(i_scale.z, i_scale.z * 1.25 - i_line_width * 2, i_scales.z);

  f_position = u_primary_model * u_secondary_model *
               (vec4(quaternion_rotation(i_position * scale, i_rotation), 1.0)
                + vec4(i_translation, 0.0));
  gl_Position = u_pv * f_position;
  f_color = i_color * 0.003921569; // 0.003921569 ~= 1/255
  f_normal = quaternion_rotation(i_normal, i_rotation);
  f_uv = i_uv;
}
