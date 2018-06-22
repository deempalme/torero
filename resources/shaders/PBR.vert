#version 420 core
// GLSL vertex shader with physically based rendering and fog

layout(location = 0) in vec3 i_position;   // vertex position
layout(location = 1) in vec3 i_normal;     // vertex normal vector
layout(location = 3) in vec3 i_tangent;    // vertex tangent vector
layout(location = 4) in vec3 i_bitangent;  // vertex bitangent vector
layout(location = 2) in vec2 i_uv;         // vertex texture coordinates

// output values for fragment shader
out vec2 o_uv;
out vec3 o_position;
out mat3 o_TBN;

// translation, rotation and scale transformations for scene and object
uniform mat4 u_scene_model;
uniform mat4 u_object_model;
// view matrix
uniform mat4 u_view;
// projection matrix
uniform mat4 u_projection;

void main()
{
  o_uv = i_uv;
  mat4 model = u_scene_model * u_object_model;
  o_position = vec3(model * vec4(i_position, 1.0));

  mat4 rotation = mat4(mat3(model));

  o_TBN = mat3(normalize(vec3(rotation * vec4(i_tangent.xyz, 0.0))),
               normalize(vec3(rotation * vec4(i_bitangent.xyz, 0.0))),
               normalize(vec3(rotation * vec4(i_normal.xyz, 0.0))) );

  gl_Position =  u_projection * u_view * vec4(o_position, 1.0);
}
