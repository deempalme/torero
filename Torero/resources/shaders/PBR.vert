#version 420
// GLSL vertex shader with physically based rendering and fog

in vec3 i_position;   // vertex position
in vec2 i_texture;    // vertex's texture coordinates
in vec3 i_normal;     // vertex normal vector

// output values for fragment shader
out vec4 o_position;
out vec2 o_texture;
out vec4 o_world_position;
out vec3 o_normal;
out bool o_colored;
out bool o_is_interior;
// precalculated values for light reflections
out vec3 o_V;
out vec3 o_L[5];
out vec3 o_H[5];
out vec3 o_radiance[5];

// translation, rotation and scale transformations for scene and object
uniform mat4 u_scene_model;
uniform mat4 u_object_model;
// view matrix
uniform mat4 u_view;
// projection matrix
uniform mat4 u_projection;
// light and camera positions
uniform vec3 u_light[5];
uniform vec3 u_light_color[5];
uniform vec3 u_camera;
// indicates if the object is inside the vehicle
//uniform bool u_is_interior;

// calculating vectors for light reflections' calculation inside the fragment shader
void light_precalculation(){
  o_V = normalize(u_camera - o_world_position);
//  int maximum = (o_is_interior = u_is_interior)? 4 : 3;
//  int minimum = (o_is_interior)? 3 : 0;

//  for(int i = minimum; i < maximum; i++){
  for(int i = 0; i < 5; i++){
    o_L[i] = normalize(u_light[i] - o_world_position);
    o_H[i] = normalize(o_V + o_L[i]);
    float distance = length(u_light[i] - o_world_position);
    float attenuation = 1.0 / (distance * distance);
    o_radiance[i] = u_light_color[i] * attenuation;
  }
}

void main(void)
{
  mat4 model = u_scene_model * u_object_model;
  o_world_position = model * vec4(i_position, 1.0);
  o_normal = normalize(mat3(model) * i_normal);
  o_position = u_view * o_world_position;

  o_texture = i_texture;

  // optimizing fragment calculations with the following line
  light_precalculation();

  gl_Position = u_projection * o_position;
}
