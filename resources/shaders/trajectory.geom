#version 420 core
// Trajectory geometry shader

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 7) out;

in vec4 g_color[];
in float g_line_width[];
in float g_distance[];
in float g_angle[];

uniform mat4 u_pv;

out vec3 f_position;
out vec3 f_normal;
out vec4 f_color;
out vec2 f_uv;

const vec3 normal = vec3(0.0, 1.0, 0.0);

vec3 rotate_z(vec3 vector, float angle){
  float s = sin(angle);
  float c = cos(angle);
  vec3 result = vector;

  result.x = vector.x * c - vector.y * s;
  result.y = vector.x * s + vector.y * c;

  return result;
}

void main(void)
{
  // get the four vertices passed to the shader:
  vec3 p0 = gl_in[0].gl_Position.xyz;	// start of previous segment
  vec3 p1 = gl_in[1].gl_Position.xyz;	// end of previous segment, start of current segment
  vec3 p2 = gl_in[2].gl_Position.xyz;	// end of current segment, start of next segment
  vec3 p3 = gl_in[3].gl_Position.xyz;	// end of next segment

  // determine the direction of each of the 3 segments (previous, current, next)
  vec2 v0, v1, v2;
  v1 = normalize(p2.xz - p1.xz);

  if(p0 == p1)
    v0 = v1;
  else
    v0 = normalize(p1.xz - p0.xz);
  if(p3 == p2)
    v2 = v1;
  else
    v2 = normalize(p3.xz - p2.xz);

  // determine the normal of each of the 3 segments (previous, current, next)
  vec2 n0 = vec2(-v0.y, v0.x);
  vec2 n1 = vec2(-v1.y, v1.x);
  vec2 n2 = vec2(-v2.y, v2.x);

  // determine miter lines by averaging the normals of the 2 segments
  vec2 miter_a = normalize(n0 + n1);	// miter at start of current segment
  vec2 miter_b = normalize(n1 + n2);	// miter at end of current segment

  float thickness1 = g_line_width[1]/2.0;
  float thickness2 = g_line_width[2]/2.0;

  float distance1 = g_distance[1];
  float distance2 = g_distance[2];

  // determine the length of the miter by projecting it onto normal and then inverse it
  float length_a = thickness1 / dot(miter_a, n1);
  float length_b = thickness2 / dot(miter_b, n2);

  vec3 normal1 = rotate_z(normal, g_angle[1]);
  vec3 normal2 = rotate_z(normal, g_angle[2]);

  vec3 width1, width2;

  f_color = g_color[1]/255.0;

  // prevent excessively long miters at sharp corners
  if(dot(v0, v1) < -0.75){
    miter_a = n1;
    length_a = thickness1;

    width1 = rotate_z(vec3(thickness1 * n0.x, 0.0, thickness1 * n0.y), g_angle[1]);
    width2 = rotate_z(vec3(thickness1 * n1.x, 0.0, thickness1 * n1.y), g_angle[2]);

    f_normal = normal1;

    // close the gap
    if(dot(v0, n1) > 0){
      f_uv = vec2(distance1, 0);
      gl_Position = u_pv * vec4(p1 + width1, 1.0);
      f_position = gl_Position.xyz;
      EmitVertex();

      f_uv = vec2(distance1, 0);
      gl_Position = u_pv * vec4(p1 + width2, 1.0);
      f_position = gl_Position.xyz;
      EmitVertex();

      f_uv = vec2(distance1, 0.5);
      gl_Position = u_pv * vec4(p1, 1.0);
      f_position = gl_Position.xyz;
      EmitVertex();

      EndPrimitive();
    }else{
      f_uv = vec2(distance1, 1);
      gl_Position = u_pv * vec4(p1 - width2, 1.0);
      f_position = gl_Position.xyz;
      EmitVertex();

      f_uv = vec2(distance1, 1);
      gl_Position = u_pv * vec4(p1 - width1, 1.0);
      f_position = gl_Position.xyz;
      EmitVertex();

      f_uv = vec2(distance1, 0.5);
      gl_Position = u_pv * vec4(p1, 1.0);
      f_position = gl_Position.xyz;
      EmitVertex();

      EndPrimitive();
    }
  }

  if(dot(v1, v2) < -0.75){
    miter_b = n1;
    length_b = thickness2;
  }

  width1 = rotate_z(vec3(length_a * miter_a.x, 0.0, length_a * miter_a.y), g_angle[1]);
  width2 = rotate_z(vec3(length_b * miter_b.x, 0.0, length_b * miter_b.y), g_angle[2]);

  f_normal = normal1;

  // generate the triangle strip
  f_uv = vec2(distance1, 0);
  gl_Position = u_pv * vec4(p1 + width1, 1.0);
  f_position = gl_Position.xyz;
  EmitVertex();

  f_uv = vec2(distance1, 1);
  gl_Position = u_pv * vec4(p1 - width1, 1.0);
  f_position = gl_Position.xyz;
  EmitVertex();

  f_color = g_color[2]/255.0;
  f_normal = normal2;

  f_uv = vec2(distance2, 0);
  gl_Position = u_pv * vec4(p2 + width2, 1.0);
  f_position = gl_Position.xyz;
  EmitVertex();

  f_uv = vec2(distance2, 1);
  gl_Position = u_pv * vec4(p2 - width2, 1.0);
  f_position = gl_Position.xyz;
  EmitVertex();

  EndPrimitive();
}
