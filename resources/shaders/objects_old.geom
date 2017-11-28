#version 420 core
// Obstacles geometry shader

layout(points) in;
layout(triangle_strip, max_vertices = 72) out;

in VS_OUT {
  vec4 o_a_s_l_t;
  vec4 o_color;
  mat4 o_model;
  mat4 o_arrow_model;
} gs_in[];

uniform mat4 u_pv;

out vec3 f_position;
out vec3 f_normal;
out vec4 f_color;

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

const vec3 box_normals[6] = vec3[6](
  vec3(-1.00, 0.00, 0.00),  // left
  vec3( 1.00, 0.00, 0.00),  // right
  vec3( 0.00,-1.00, 0.00),  // down
  vec3( 0.00, 1.00, 0.00),  // up
  vec3( 0.00, 0.00,-1.00),  // back
  vec3( 0.00, 0.00, 1.00)   // front
);

// ------------------------------------------------------------------------------------ //
// ------------------------------------ SOLID BOX ------------------------------------- //
// ------------------------------------------------------------------------------------ //
const vec3 box[8] = vec3[8](
  vec3(-0.50,-0.50, 0.50),
  vec3(-0.50, 0.50, 0.50),
  vec3( 0.50,-0.50, 0.50),
  vec3( 0.50, 0.50, 0.50),

  vec3(-0.50,-0.50,-0.50),
  vec3(-0.50, 0.50,-0.50),
  vec3( 0.50,-0.50,-0.50),
  vec3( 0.50, 0.50,-0.50)
);

const int box_indices[36] = int[36](
  0, 0, 1, 2, 3, 3, // front
  2, 2, 3, 6, 7, 7, // right
  6, 6, 7, 4, 5, 5, // back
  4, 4, 5, 0, 1, 1, // left
  0, 0, 2, 4, 6, 6, // bottom
  3, 3, 1, 7, 5, 5  // top
);

const int box_n_i[36] = int[36](
  5, 5, 5, 5, 5, 5,
  1, 1, 1, 1, 1, 1,
  4, 4, 4, 4, 4, 4,
  0, 0, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3
);

void build_box(){
  f_color = gs_in[0].o_color;
  for(int i = 0; i < 36; i++){
    f_normal = normalize(gs_in[0].o_model * vec4(box_normals[box_n_i[i]], 1.0)).xyz;
    vec4 position = gl_in[0].gl_Position + gs_in[0].o_model * vec4(box[box_indices[i]], 1.0);
    f_position = position.xyz;
    gl_Position = u_pv * position;
    EmitVertex();
  }
  EndPrimitive();
}
// ------------------------------------------------------------------------------------ //
// ----------------------------------- HOLLOW BOX ------------------------------------- //
// ------------------------------------------------------------------------------------ //

const int hollow_box_indices[72] = int[72](
  6, 6, 20, 7, 21, 1, 23, 0, 22, 6, 20, 20, // left
  2, 2, 16, 3, 17, 5, 19, 4, 18, 2, 16, 16, // right
  6, 6, 12, 0, 13, 2, 15, 4, 14, 6, 12, 12, // bottom
  1, 1,  8, 7,  9, 5, 11, 3, 10, 1,  8,  8, // top
  4, 4, 28, 5, 29, 7, 31, 6, 30, 4, 28, 28, // back
  0, 0, 24, 1, 25, 3, 27, 2, 26, 0, 24, 24  // front
);

const int hollow_box_n_i[72] = int[72](
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
);

void build_hollow_box(){
  const float lx = gl_in[0].gl_PointSize;
  const float ly = gl_in[0].gl_PointSize;
  const float lz = gl_in[0].gl_PointSize;

  const vec3 hollow_box[32] = vec3[32](
    // front
    vec3(-0.50,-0.50, 0.50),  // 0
    vec3(-0.50, 0.50, 0.50),  // 1
    vec3( 0.50,-0.50, 0.50),  // 2
    vec3( 0.50, 0.50, 0.50),  // 3
    // back
    vec3( 0.50,-0.50,-0.50),  // 4
    vec3( 0.50, 0.50,-0.50),  // 5
    vec3(-0.50,-0.50,-0.50),  // 6
    vec3(-0.50, 0.50,-0.50),  // 7
    // inside top
    vec3(-0.50 + lx, 0.50, 0.50 - lz),  // 8
    vec3(-0.50 + lx, 0.50,-0.50 + lz),  // 9
    vec3( 0.50 - lx, 0.50, 0.50 - lz),  // 10
    vec3( 0.50 - lx, 0.50,-0.50 + lz),  // 11
    // inside bottom
    vec3(-0.50 + lx,-0.50,-0.50 + lz),  // 12
    vec3(-0.50 + lx,-0.50, 0.50 - lz),  // 13
    vec3( 0.50 - lx,-0.50,-0.50 + lz),  // 14
    vec3( 0.50 - lx,-0.50, 0.50 - lz),  // 15
    // inside right
    vec3( 0.50,-0.50 + ly, 0.50 - lz),  // 16
    vec3( 0.50, 0.50 - ly, 0.50 - lz),  // 17
    vec3( 0.50,-0.50 + ly,-0.50 + lz),  // 18
    vec3( 0.50, 0.50 - ly,-0.50 + lz),  // 19
    // inside left
    vec3(-0.50,-0.50 + ly,-0.50 + lz),  // 20
    vec3(-0.50, 0.50 - ly,-0.50 + lz),  // 21
    vec3(-0.50,-0.50 + ly, 0.50 - lz),  // 22
    vec3(-0.50, 0.50 - ly, 0.50 - lz),  // 23
    // inside front
    vec3(-0.50 + lx,-0.50 + ly, 0.50),  // 24
    vec3(-0.50 + lx, 0.50 - ly, 0.50),  // 25
    vec3( 0.50 - lx,-0.50 + ly, 0.50),  // 26
    vec3( 0.50 - lx, 0.50 - ly, 0.50),  // 27
    // inside back
    vec3( 0.50 - lx,-0.50 + ly,-0.50),  // 28
    vec3( 0.50 - lx, 0.50 - ly,-0.50),  // 29
    vec3(-0.50 + lx,-0.50 + ly,-0.50),  // 30
    vec3(-0.50 + lx, 0.50 - ly,-0.50)   // 31
  );

  f_color = gs_in[0].o_color;
  for(int i = 0; i < 72; i++){
    f_normal = normalize(gs_in[0].o_model * vec4(box_normals[hollow_box_n_i[i]], 1.0)).xyz;
    vec4 position = gl_in[0].gl_Position +
                    gs_in[0].o_model * vec4(hollow_box[hollow_box_indices[i]], 1.0);
    f_position = position.xyz;
    gl_Position = u_pv * position;
    EmitVertex();
  }
  EndPrimitive();
}

void main(){
  //build_box();
  build_hollow_box();
}
