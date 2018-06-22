#version 420 core
// GLSL vertex shader with physically based rendering and fog

layout(location = 0) in vec3 i_position;   // vertex position
layout(location = 1) in vec3 i_normal;     // vertex normal vector
layout(location = 2) in vec2 i_uv;         // vertex texture coordinates

// output values for fragment shader
out vec4 o_position;
out vec2 o_uv;
out vec3 o_world_position;
out vec3 o_normal;
// precalculated values for light reflections
out vec3 o_V;
out vec3 o_L[7];
out vec3 o_H[7];
out vec3 o_radiance[7];
out float o_light_size;

// translation, rotation and scale transformations for scene and object
uniform mat4 u_scene_model;
uniform mat4 u_object_model;
// view matrix
uniform mat4 u_view;
// projection matrix
uniform mat4 u_projection;
// light and camera positions
uniform vec3 u_light[10];
uniform vec3 u_light_color[10];
uniform int u_light_size;
uniform vec3 u_camera;
// parallel light
uniform vec3 u_sun;
uniform vec3 u_sun_color;

// calculating vectors for light reflections' calculation inside the fragment shader
void light_precalculation(){
  o_V = normalize(u_camera - o_world_position);
  o_light_size = u_light_size + 1;

  for(int i = 0; i < u_light_size; i++){
    o_L[i] = normalize(u_light[i] - o_world_position);
    o_H[i] = normalize(o_V + o_L[i]);
    float distance = length(u_light[i] - o_world_position);
    float attenuation = 1.0 / (distance * distance);
    o_radiance[i] = u_light_color[i] * attenuation;
  }
  o_L[u_light_size] = normalize(u_sun);
  o_H[u_light_size] = normalize(o_V + o_L[u_light_size]);
  o_radiance[u_light_size] = u_sun_color;
}

void main(void)
{
  mat4 model = u_scene_model * u_object_model;
  o_world_position = vec3(model * vec4(i_position, 1.0));
  o_normal = normalize(mat3(model) * i_normal);
  o_position = u_view * vec4(o_world_position, 1.0);

  o_uv = i_uv;

  // optimizing fragment calculations with the following line
  light_precalculation();

  gl_Position = u_projection * o_position;
}
