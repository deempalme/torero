#version 420 core
// Text geometry shader

layout(points) in;
layout(points, max_vertices = 4) out;

//in vec3 g_position[];
//in vec2 g_offset[];
//in vec3 g_size_angle[];
//in vec4 g_color[];
//in vec4 g_texture[];

//out vec3 f_position;
out vec4 f_color;
//out vec2 f_uv;

//uniform vec3 u_size_soft_weight;

uniform mat4 u_view;
uniform mat4 u_projection;

//uniform vec2 u_window;

vec2 rotate_z(vec2 vector, float angle){
  float s = sin(angle);
  float c = cos(angle);
  vec2 result = vector;

  result.x = vector.x * c - vector.y * s;
  result.y = vector.x * s + vector.y * c;

  return result;
}

void main(void)
{
//  const vec2 origin = gl_in[0].gl_Position.xy;
//  const vec2 end = gl_in[0].gl_Position.wz;

//  if(gl_in[0].gl_Position.z < 0.0)
//    f_color = vec4(1.0, 0.0, 0.0, 1.0);
//    return;

//  f_color = g_color[0];
  f_color = vec4(1.0);

  gl_PointSize = 5.0;

  // First square's point (lower left corner):
//  f_uv = vec2(0.0, g_texture[0].y);
  gl_Position = u_projection * u_view * (gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.0, 0.0));
//  gl_Position = vec4(-0.1,-0.1, end.x, end.y);
//  f_position = g_position[0];
  EmitVertex();

  gl_Position = u_projection * u_view * (gl_in[0].gl_Position + vec4(-0.5,-0.5, 0.0, 0.0));
  EmitVertex();

  gl_Position = u_projection * u_view * (gl_in[0].gl_Position + vec4( 0.5, 0.5, 0.0, 0.0));
  EmitVertex();

  gl_Position = u_projection * u_view * (gl_in[0].gl_Position + vec4( 0.5,-0.5, 0.0, 0.0));
  EmitVertex();

//  f_uv = vec2(g_texture[0].x, g_texture[0].y);
//  gl_Position = vec4(-0.1, 0.1, end.x, end.y);
//  f_position = g_position[0];
//  EmitVertex();

//  f_uv = vec2(g_texture[0].z, 0.0);
//  gl_Position = vec4( 0.1,-0.1, end.x, end.y);
//  f_position = g_position[0];
//  EmitVertex();

//  f_uv = vec2(g_texture[0].z, g_texture[0].w);
//  gl_Position = vec4( 0.1, 0.1, end.x, end.y);
//  f_position = g_position[0];
//  EmitVertex();

  /*
  // First square's point (lower left corner):
  f_uv = vec2(0.0, g_texture[0].y);
  vec2 position = rotate_z(g_size_angle[0].xy * -0.5 * u_size_soft_weight.x,
                           g_size_angle[0].z) + g_offset[0];
  gl_Position = gl_Position + vec4(position.x/u_window.x, position.y/u_window.y, 0.0, 0.0);
  f_position = g_position[0];
  EmitVertex();

  f_uv = vec2(g_texture[0].x, g_texture[0].y);
  position = rotate_z(vec2(-g_size_angle[0].x, g_size_angle[0].y) * 0.5 * u_size_soft_weight.x,
                      g_size_angle[0].z) + g_offset[0];
  gl_Position = gl_Position + vec4(position.x/u_window.x, position.y/u_window.y, 0.0, 0.0);
  f_position = g_position[0];
  EmitVertex();

  f_uv = vec2(g_texture[0].z, 0.0);
  position = rotate_z(vec2(g_size_angle[0].x, -g_size_angle[0].y) * 0.5 * u_size_soft_weight.x,
                      g_size_angle[0].z) + g_offset[0];
  gl_Position = gl_Position + vec4(position.x/u_window.x, position.y/u_window.y, 0.0, 0.0);
  f_position = g_position[0];
  EmitVertex();

  f_uv = vec2(g_texture[0].z, g_texture[0].w);
  position = rotate_z(g_size_angle[0].xy * 0.5 * u_size_soft_weight.x,
                      g_size_angle[0].z) + g_offset[0];
  gl_Position = gl_Position + vec4(position.x/u_window.x, position.y/u_window.y, 0.0, 0.0);
  f_position = g_position[0];
  EmitVertex();
  */

  EndPrimitive();
}
