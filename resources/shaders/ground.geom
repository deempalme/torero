#version 420 core
// Ground geometry shader

layout(points) in;
layout(triangle_strip, max_vertices = 30) out;

in vec4 g_color[];
in vec2 g_dimension[];
in float g_height[];

uniform mat4 u_primary_model;
uniform mat4 u_secondary_model;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform int u_fog;
uniform int u_2D;
uniform int u_free;

uniform float u_width;
uniform float u_length;
uniform vec3 u_position;

out vec3 f_position;
out vec3 f_normal;
out vec4 f_color;

// ------------------------------------------------------------------------------------ //
// ------------------------------------ SOLID BOX ------------------------------------- //
// ------------------------------------------------------------------------------------ //
/*
*                      THE BOX
*          5 –––––––––––––––––––––––––––– 7
*         /|                             /|
*        / |                            / |
*       /  |                           /  |
*      /   |                          /   |
*     /    |                         /    |
*    1 –––––––––––––––––––––––––––– 3     |
*    |     |                        |     |
*    |     |                        |     |   -z
*    |     |                        |     |   /
*    |     |           (*)          |     |  /
*    |     |                        |     | /
*    |     |                        |     |/
*    |     4 –––––––––––––––––––––––|–––– 6 ––––––– x
*    |    /                         |    /|
*    |   /                          |   / |
*    |  /                           |  /  |
*    | /                            | /   |
*    |/                             |/   -y
*    0 –––––––––––––––––––––––––––– 2
*/
void build_box(mat4 model, mat4 pv, vec4 position, vec2 dimension){
  const float width = dimension.x/2.0;
  const float length = dimension.y/2.0;

  const vec4 box[8] = vec4[8](
    model * vec4(-width + 0.001, 0.0,         length - 0.001, 0.0),
    model * vec4(-width + 0.001, g_height[0], length - 0.001, 0.0),
    model * vec4( width - 0.001, 0.0,         length - 0.001, 0.0),
    model * vec4( width - 0.001, g_height[0], length - 0.001, 0.0),

    model * vec4(-width + 0.001, 0.0,        -length + 0.001, 0.0),
    model * vec4(-width + 0.001, g_height[0],-length + 0.001, 0.0),
    model * vec4( width - 0.001, 0.0,        -length + 0.001, 0.0),
    model * vec4( width - 0.001, g_height[0],-length + 0.001, 0.0)
  );

  const int box_indices[30] = int[30](
    4, 4, 5, 0, 1, 1, // left
    2, 2, 3, 6, 7, 7, // right
    3, 3, 1, 7, 5, 5, // top
    6, 6, 7, 4, 5, 5, // back
    0, 0, 1, 2, 3, 3  // front
  );

  const int box_n_i[30] = int[30](
    0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4
  );

  const vec3 box_normals[5] = vec3[5](
    normalize(model * vec4(-1.0, 0.0, 0.0, 0.0)).xyz, // left
    normalize(model * vec4( 1.0, 0.0, 0.0, 0.0)).xyz, // right
    normalize(model * vec4( 0.0, 1.0, 0.0, 0.0)).xyz, // up
    normalize(model * vec4( 0.0, 0.0,-1.0, 0.0)).xyz, // back
    normalize(model * vec4( 0.0, 0.0, 1.0, 0.0)).xyz  // front
  );

  for(int i = 0; i < 30; i++){
    f_normal = box_normals[box_n_i[i]];
    vec4 position_2 = position + box[box_indices[i]];
    f_position = position_2.xyz;
    gl_Position = pv * position_2;
    EmitVertex();
  }
  EndPrimitive();
}
// ------------------------------------------------------------------------------------ //
// ------------------------------------ SOLID SQUARE ---------------------------------- //
// ------------------------------------------------------------------------------------ //
/*
*                      THE SQUARE
*          1 –––––––––––––––––––––––––––– 3
*         /                              /
*        /                              /
*       /             (*)              /
*      /                              /
*     /                              /
*    0 –––––––––––––––––––––––––––– 2
*
*/
void build_square(mat4 model, mat4 pv, vec4 position, vec2 dimension){
  const float width = dimension.x/2.0;
  const float length = dimension.y/2.0;

  const vec4 square[4] = vec4[4](
    model * vec4(-width, 0.0, length, 0.0),
    model * vec4(-width, 0.0,-length, 0.0),
    model * vec4( width, 0.0, length, 0.0),
    model * vec4( width, 0.0,-length, 0.0)
  );

  const int square_indices[6] = int[6](
    0, 0, 1, 2, 3, 3
  );

  f_normal = normalize(model * vec4(0.0, 1.0, 0.0, 0.0)).xyz;

  for(int i = 0; i < 6; i++){
    vec4 position_2 = position + square[square_indices[i]];
    f_position = position_2.xyz;
    gl_Position = pv * position_2;
    EmitVertex();
  }
  EndPrimitive();
}
// ------------------------------------------------------------------------------------ //
// ---------------------------------------- FOG --------------------------------------- //
// ------------------------------------------------------------------------------------ //
float calcule_fog(vec4 position, float alpha){
  const float density = 0.010;
  const float gradient = 3.0;

  vec4 position_relative_to_camera = u_view * position;

  float distance = length(position_relative_to_camera.xyz);
  float visibility = exp(-pow((distance * density), gradient));
  visibility = clamp(visibility, 0.0, 1.0);

  return alpha * visibility;
}
// ------------------------------------------------------------------------------------ //
void main(){
  mat4 model = u_primary_model * u_secondary_model;
  mat4 rotation = mat4(mat3(model));
  mat4 pv = u_projection * u_view;

  vec4 position;

  if(u_free == 1){
    position = model * vec4(-gl_in[0].gl_Position.z,
                             gl_in[0].gl_Position.y - 0.005,
                            -gl_in[0].gl_Position.x,
                             1.0);
  }else{
    position = model * vec4(-u_position.y + gl_in[0].gl_Position.x * u_width,
                             u_position.z - 0.005,
                            -u_position.x + gl_in[0].gl_Position.z * u_length,
                             1.0);
  }

  f_color = vec4(g_color[0].xyz, mix(g_color[0].a, calcule_fog(position, g_color[0].a), u_fog));

  if(g_height[0] > -0.005 && g_height[0] < 0.005 || u_2D == 1)
    build_square(rotation, pv, position, mix(vec2(u_width, u_length), g_dimension[0], u_free));
  else
    build_box(rotation, pv, position, mix(vec2(u_width, u_length), g_dimension[0], u_free));
}
