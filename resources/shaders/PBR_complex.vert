#version 420 core
// GLSL vertex shader with physically based rendering and fog

layout(location = 0) in vec3 i_position;   // vertex position
layout(location = 1) in vec3 i_normal;     // vertex normal vector
layout(location = 2) in vec2 i_uv;         // vertex texture coordinates
layout(location = 3) in vec3 i_tangent;    // vertex tangent vector

// output values for fragment shader
out vec2 o_uv;
out vec3 o_position;
out mat3 o_TBN;
out vec3 o_camera_position;

layout (std140, binding = 1) uniform u_scene {
  mat4 u_projection;
  mat4 u_view;
  mat4 u_pv;
  mat4 u_psv;
  vec3 u_camera_position;
};

// This shader accepts up to 2 additional rotations
// u_quaternion[0] is multiplied first
uniform vec4 u_quaternion[2];
// Components X and Y indicate if object is using the additional quaternions 0 and 1 respectively.
// value < 0.5 : does not multiplies vertex by the respective quaternion
uniform vec2 u_quaternion_usage;

// Object positioning relative to FRAME
uniform vec4 u_object_rotation;
uniform vec3 u_object_translation;
// FRAME positioning relative to WORLD coordinate 0, 0
uniform vec4 u_frame_rotation;
uniform vec3 u_frame_translation;

// Vector rotation using quaternions
vec3 quaternion_rotation(const vec3 v, const vec4 q){
  if(all(equal(q, vec4(0.0, 0.0, 0.0, 1.0))))
    return v;
  return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

void main(){

  o_uv = i_uv;
  o_position = i_position;
  o_camera_position = u_camera_position;
  vec3 normal = i_normal;
  vec3 tangent = i_tangent;

  if(u_quaternion_usage.x > 0.5){
    o_position = quaternion_rotation(o_position, u_quaternion[0]);
    normal = quaternion_rotation(normal, u_quaternion[0]);
    tangent = quaternion_rotation(tangent, u_quaternion[0]);
  }

  if(u_quaternion_usage.y > 0.5){
    o_position = quaternion_rotation(o_position, u_quaternion[1]);
    normal = quaternion_rotation(normal, u_quaternion[1]);
    tangent = quaternion_rotation(tangent, u_quaternion[1]);
  }

  // Object positioning
  o_position = quaternion_rotation(o_position, u_object_rotation) + u_object_translation;
  normal   = quaternion_rotation(normal, u_object_rotation) + u_object_translation;
  tangent  = quaternion_rotation(tangent, u_object_rotation) + u_object_translation;
  // Frame positioning
  o_position = quaternion_rotation(o_position, u_frame_rotation) + u_frame_translation;
  normal   = normalize(quaternion_rotation(normal, u_frame_rotation));
  tangent  = normalize(quaternion_rotation(tangent, u_frame_rotation));

  tangent = normalize(tangent - dot(tangent, normal) * normal);
  vec3 bitangent = -normalize(cross(normal, tangent));

  o_TBN = transpose(mat3(tangent, bitangent, normal));

  gl_Position =  u_pv * vec4(o_position, 1.0);
}
