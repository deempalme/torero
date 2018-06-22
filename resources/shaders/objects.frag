#version 420 core
// Objects fragment shader

in vec4 f_position;
in vec3 f_normal;
in vec4 f_color;
in vec2 f_uv;

out vec4 frag_color;

// lights
uniform vec3 u_point_light[4];
uniform vec3 u_point_light_color[4];
// directional light
uniform vec3 u_directional_light;
uniform vec3 u_directional_light_color;
// camera positions
uniform vec3 u_camera_position;
// ambient occlusion texture
uniform sampler2D u_ao;
// is it solid?
uniform float u_solid;

const float shininess = 16.0;
const float energy = (2.0 + shininess) / (2.0 * 3.14159265);

vec3 calculate_point_light(const vec3 light, const vec3 color,
                           const vec3 viewDir){
  // ambient
  const vec3 ambient = 0.3 * f_color.rgb;

//  vec3 lightDir = normalize(light - f_position.xyz);
  const vec3 lightDir = normalize(light);
  const float diff = max(dot(lightDir, f_normal), 0.0);
  const vec3 diffuse = diff * f_color.rgb * 0.7;
  // phong light
  const vec3 reflectDir = reflect(-lightDir, f_normal);
  const float spec = energy * pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  const vec3 specular = color * spec;
  // attenuation
//  float distance = length(light - f_position.xyz);
  const float distance = length(light);
  const float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
  // combine results
  return (ambient + diffuse + specular) * attenuation;
}

vec3 calculate_directional_light(const vec3 light, const vec3 color,
                                 const vec3 viewDir){
  // ambient
  vec3 ambient = 0.3 * f_color.rgb;

  vec3 lightDir = normalize(light);
  float diff = max(dot(lightDir, f_normal), 0.0);
  vec3 diffuse = diff * f_color.rgb * 0.7;
  // phong light
  vec3 reflectDir = reflect(-lightDir, f_normal);
//  float spec = energy * pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  float spec = max(dot(viewDir, reflectDir), 0.0);
  //specular
  vec3 specular = spec * 0.3 * color;
  // combine results
  return (ambient + diffuse + specular);
}

void main()
{
  float ao = texture(u_ao, f_uv).r;
  vec3 color = vec3(0.0);
  // view direction
  vec3 viewDir = normalize(u_camera_position - f_position.xyz);

  for(int i = 0; i < 4; i++){
    color += calculate_point_light(u_point_light[i], u_point_light_color[i],
                                   viewDir);
  }
  color += calculate_directional_light(u_directional_light,
                                       u_directional_light_color,
                                       viewDir);

  frag_color = vec4(color * max(ao, u_solid), f_color.a);
}
