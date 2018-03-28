#version 420 core
// GUI Text geometry shader

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 g_size[];
in vec4 g_color[];
in vec4 g_texture[];

out vec4 f_color;
out vec2 f_uv;

// translation and rotation transformations
uniform vec4 u_rotation;
uniform vec3 u_translation;
// projection matrix
uniform mat4 u_projection;

// Rotate vector using quaternions
vec3 quat_rot(vec4 q, vec3 v){
  return v + 2.0 * cross(cross(v, q.xyz) + q.w * v, q.xyz);
}


void main(void)
{
  const vec3 position = gl_in[0].gl_Position.xyz;
  f_color = g_color[0];

  // First square's point (lower left corner):
  f_uv = g_texture[0].xw;
  gl_Position = u_projection * vec4(quat_rot(u_rotation,
                                             vec3(position.x,
                                                  position.y - g_size[0].y,
                                                  position.z)) + u_translation, 1.0);
  EmitVertex();

  // Upper left corner:
  f_uv = g_texture[0].xy;
  gl_Position = u_projection * vec4(quat_rot(u_rotation,
                                             vec3(position.x,
                                                  position.y,
                                                  position.z)) + u_translation, 1.0);
  EmitVertex();

  // Lower right corner
  f_uv = g_texture[0].zw;
  gl_Position = u_projection * vec4(quat_rot(u_rotation,
                                             vec3(position.x + g_size[0].x,
                                                  position.y - g_size[0].y,
                                                  position.z)) + u_translation, 1.0);
  EmitVertex();

  // Upper right corner
  f_uv = g_texture[0].zy;
  gl_Position = u_projection * vec4(quat_rot(u_rotation,
                                             vec3(position.x + g_size[0].x,
                                                  position.y,
                                                  position.z)) + u_translation, 1.0);
  EmitVertex();

  EndPrimitive();
}
