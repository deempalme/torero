#version 420 core
// GLSL vertex shader with physically based rendering and fog

layout(location = 0) in vec3 i_position;   // vertex position
layout(location = 1) in vec3 i_normal;     // vertex normal vector
layout(location = 2) in vec2 i_uv;         // vertex texture coordinates

// output values for fragment shader
out vec2 o_uv;
out vec3 o_world_position;
out vec3 o_normal;

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
  o_world_position = vec3(model * vec4(i_position, 1.0));
  o_normal = mat3(model) * i_normal;

  gl_Position =  u_projection * u_view * vec4(o_world_position, 1.0);
}
