#version 420 core
#extension GL_EXT_shader_texture_lod: enable
// GLSL fragment shader with physically based rendering

in vec2 o_uv;
in vec3 o_position;
in mat3 o_TBN;
in vec3 o_camera_position;

// 2D textures
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_pbr_emission;
// metallic effect
uniform int u_metallized;
uniform float u_metallic_value;
// roughness reflectiviness
uniform int u_roughed;
uniform float u_roughness_value;
// use color instead of texture
uniform int u_colored;
uniform vec4 u_color;
// irradiance maps
uniform samplerCube u_irradiance;
uniform samplerCube u_prefilter;
uniform sampler2D u_brdf_LUT;
// lights
uniform vec3 u_light[4];
uniform vec3 u_light_color[4];

//output color
layout (location = 0) out vec4 o_frag_color;

const float PI = 3.14159265359;
const int LIGHT_COUNT = 4;

// ----------------------------------------------------------------------------
float distribution_GGX(vec3 N, vec3 H, float roughness){
  float a2 = roughness * roughness;
  a2 = a2 * a2;
  float NoH2 = max(dot(N, H), 0.0);
  NoH2 = NoH2 * NoH2;

  float denom = (NoH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return a2 / denom;
}
// ----------------------------------------------------------------------------
float geometry_schlick_GGX(float NoV, float roughness){
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float denom = NoV * (1.0 - k) + k;

  return NoV / denom;
}
// ----------------------------------------------------------------------------
float geometry_smith(float NoV, float NoL, float roughness){
  float ggx2 = geometry_schlick_GGX(NoV, roughness);
  float ggx1 = geometry_schlick_GGX(NoL, roughness);

  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnel_schlick(vec3 H, vec3 V, vec3 F0){
  float VoH = max(dot(V, H), 0.0);
  return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * VoH  - 6.98316) * VoH);
}

vec3 fresnel_schlick_roughness(float NoV, vec3 F0, float roughness){
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(2.0, (-5.55473 * NoV  - 6.98316) * NoV);
}
// ----------------------------------------------------------------------------
void main()
{
  // material properties
  vec4 albedo = u_colored > 0 ? u_color : texture(u_albedo, o_uv).rgba;
  vec4 pbr_emission = texture(u_pbr_emission, o_uv);
  vec3 N = texture(u_normal, o_uv).xyz * 2.0 - 1.0;

  albedo.rgb = pow(albedo.rgb, vec3(2.2));
  float metallic = u_metallized > 0 ? u_metallic_value : pbr_emission.r;
  float roughness = u_roughed > 0 ? u_roughness_value : pbr_emission.g;
  float cavity = pbr_emission.b;
  float emission = pbr_emission.a < 0.5 ? 1.0 : 0.0;

  // input lighting data
  N = normalize(o_TBN * N);
  vec3 V = normalize(o_camera_position - o_position);
  vec3 R = reflect(-V, N);
  float NoV = max(dot(N, V), 0.0);

  // calculate reflectance at normal incidence; if di-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
  vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);

  // Calculating point lights ---------------------------------------------------------------
  for(int i = 0; i < LIGHT_COUNT; ++i){
    // calculate per-light radiance
    /*
    if(i == 0){
      // Directional light
      vec3 L = normalize(u_light[i]);
      float distance = length(u_light[i]);
    }else{
      // Point light
      vec3 L = normalize(u_light[i] - o_position);
      float distance = length(u_light[i] - o_position);
    }
    */
    vec3 L = normalize(u_light[i]);
    vec3 H = normalize(V + L);
    float NoL = max(dot(N, L), 0.0);

    float distance = length(u_light[i]);
//    float attenuation = 1.0 / (distance * distance);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    vec3 radiance = u_light_color[i] * attenuation;

    // Cook-Torrance BRDF
    float Dh   = distribution_GGX(N, H, roughness);
    float Glvh = geometry_smith(NoV, NoL, roughness);
    vec3 Fvh   = fresnel_schlick(H, V, F0);

    // 0.001 to prevent divide by zero.
    float denominator = 4.0 * NoV * NoL + 0.001;
    vec3 specular = Dh * Glvh * Fvh / denominator;

    // kS is equal to Fresnel
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - Fvh;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // add to outgoing radiance Lo
    Lo += (kD * albedo.rgb / PI + specular) * radiance * NoL;
    // note that we already multiplied the BRDF by the Fresnel (kS)
    // so we won't multiply by kS again
  }

  // ambient lighting (we now use IBL as the ambient term)
  vec3 F = fresnel_schlick_roughness(NoV, F0, roughness);

  // kS is equal to Fresnel
  vec3 kD = 1.0 - F;
  kD *= 1.0 - metallic;

  vec3 irradiance = texture(u_irradiance, N).rgb;
  vec3 diffuse    = irradiance * albedo.rgb;

  // sample both the pre-filter map and the BRDF lut and combine them together as per the
  // Split-Sum approximation to get the IBL specular part.
  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefiltered_color = textureLod(u_prefilter, R, roughness * MAX_REFLECTION_LOD).rgb;
  vec2 brdf = texture(u_brdf_LUT, vec2(NoV, roughness)).rg;
  vec3 specular = prefiltered_color * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * cavity;
  vec3 color = ambient + Lo;

  // HDR tonemapping
  color = color / (color + vec3(1.0));
  // gamma correct
  color = pow(color, vec3(1.0/2.2));
  color = mix(color, color * 0.2 + albedo.rgb * 0.8, emission);

  o_frag_color = vec4(color, albedo.a);
}
