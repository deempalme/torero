#version 420 core
// Physically based rendering for the GUI

layout(location = 0) in vec3 i_position;   // vertex position
layout(location = 1) in vec2 i_uv;         // vertex texture coordinates
layout(location = 2) in vec3 i_normal;     // vertex normal vector
layout(location = 3) in vec3 i_tangent;    // vertex tangent vector
layout(location = 4) in float i_angle;     // vertex rotation angle in Y axis
layout(location = 5) in vec3 i_color;      // vertex emission color

// output values for fragment shader
out vec2 o_uv;
out vec3 o_position;
out mat3 o_TBN;
out vec3 o_color;

// Orthographic projection
uniform mat4 u_projection;
// translation, rotation and scale transformations for scene and object
uniform vec4 u_rotation;
uniform vec3 u_translation;

// Rotates a vector through Z axis
vec3 rotate_z(const vec3 vector, const float angle){
  if(angle > -0.0001 && angle < 0.0001)
    return vector;

  const float s = sin(angle);
  const float c = cos(angle);
  vec3 result = vector;

  result.x = vector.x * c - vector.y * s;
  result.y = vector.x * s + vector.y * c;

  return result;
}

// Rotate vector using quaternions
vec3 quaternion_rotation(vec4 q, vec3 v){
  return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}

void main()
{
  o_uv = i_uv;
  o_position = rotate_z(i_position, i_angle);
  o_position = quaternion_rotation(u_rotation, o_position) + u_translation;
  o_color = i_color;

  o_TBN = mat3(normalize(quaternion_rotation(u_rotation, i_tangent)),
               normalize(quaternion_rotation(u_rotation, -normalize(cross(i_normal, i_tangent)))),
               normalize(quaternion_rotation(u_rotation, i_normal)) );

  gl_Position =  u_projection * vec4(o_position, 1.0);
}
