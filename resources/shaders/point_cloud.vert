#version 420 core
//point cloud vertex shader

layout(location = 0) in vec3  i_position;
layout(location = 1) in float i_intensity;
layout(location = 2) in float i_alpha;
layout(location = 5) in vec3  i_color;

out vec4 o_color;

layout (std140, binding = 1) uniform u_scene {
  mat4 u_projection;
  mat4 u_view;
  mat4 u_pv;
  mat4 u_psv;
  vec3 u_camera_position;
};
uniform vec3 u_primary_translation;
uniform vec4 u_primary_orientation;
uniform vec3 u_secondary_translation;
uniform vec4 u_secondary_orientation;

uniform vec3  u_palette[10];     //this is the color palette
uniform float u_color_size;      //this indicates the number of elements in the color palette
uniform float u_color_mode;      //this is to turn all clouds into grayscale
uniform float u_intensity_range;
uniform float u_has_alpha;

// Rotate vector using quaternions
vec3 quat_rotation(const vec3 v, const vec4 q){
  return v + 2.0 * cross(cross(v, q.xyz) - q.w * v, q.xyz);
}

void main()
{
  vec4 position = vec4(u_primary_translation
                       + quat_rotation(u_secondary_translation
                                       + quat_rotation(i_position, u_secondary_orientation),
                                       u_primary_orientation), 1.0);
  gl_Position = u_pv * position;

  // Checking if outside the frustum
  // this is done in the this shader because all vertices are points (not polygons)
  if(gl_Position.w < 0.0 || abs(gl_Position.x) > gl_Position.w
     || abs(gl_Position.y) > gl_Position.w
     || (0 > gl_Position.z && gl_Position.z > gl_Position.w)){
    gl_Position.z = gl_Position.w + 10.0;
    return;
  }

  if(u_color_mode == 0.0){
    o_color.xyz = u_palette[0];
    o_color.a = (i_intensity / u_intensity_range) * 0.9 + 0.1;
  }else if(u_color_mode == 1.0){
    o_color.a = i_intensity / u_intensity_range;

    int idx1 = 0;// Our desired color will be between these two indexes in "color".
    int idx2 = 0;
    float fract_between = 0.0;// Fraction between "idx1" and "idx2" where our value is.

    if(o_color.a <= 0.0){
      idx1 = idx2 = 0;
    }else if(o_color.a >= 1.0){
      idx1 = idx2 = int(u_color_size) - 1;
    }else{
      o_color.a = o_color.a * (u_color_size - 1.0);
      idx1 = int(floor(o_color.a));// Our desired color will be after this index.
      idx2 = idx1 + 1;// ... and before this index (inclusive).
      fract_between = o_color.a - float(idx1);// Distance between the two indexes (0-1).
    }

    o_color.xyz = (u_palette[idx2] - u_palette[idx1]) * fract_between + u_palette[idx1];
    o_color.a = 1.0;
  }else if(u_color_mode == 2.0){
    o_color.xyz = u_palette[0];
    o_color.a = 1.0;
  }else if(u_color_mode == 3.0){
    o_color.xyz = i_color;
    o_color.a = mix(1.0, i_alpha, u_has_alpha);
  }else{
    o_color = vec4(1.0);
  }
}
