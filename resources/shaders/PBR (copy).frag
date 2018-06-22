#version 420 core
// GLSL fragment shader with physically based rendering and fog

in vec4 o_position;
in vec2 o_uv;
in vec3 o_world_position;
in vec3 o_normal;
// precalculated values for light reflections
in vec3 o_V;
in vec3 o_L[7];
in vec3 o_H[7];
in vec3 o_radiance[7];
in float o_light_size;

uniform bool u_pbr;
uniform bool u_fog;
// 2D textures
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
// metallic effect
uniform bool u_metallized;
uniform sampler2D u_metallic;
uniform float u_metallic_value;
// roughness reflectiviness
uniform bool u_roughed;
uniform sampler2D u_roughness;
uniform float u_roughness_value;
// ambient occlusion texture
uniform sampler2D u_ao;
// use color instead of texture
uniform bool u_colored;
uniform vec4 u_color;
// irradiance maps
uniform samplerCube u_irradiance;
uniform samplerCube u_prefilter;
uniform sampler2D u_brdfLUT;

//output color
out vec4 frag_color;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 get_normal_from_map(){
  vec3 Q1  = dFdx(o_world_position);
  vec3 Q2  = dFdy(o_world_position);
  vec2 st1 = dFdx(o_uv);
  vec2 st2 = dFdy(o_uv);

  vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B  = -normalize(cross(o_normal, T));
  mat3 TBN = mat3(T, B, o_normal);

  return normalize(TBN * (texture(u_normal, o_uv).xyz * 2.0 - 1.0));
}
// ----------------------------------------------------------------------------
float distribution_GGX(vec3 N, vec3 H, float roughness){
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float geometry_schlick_GGX(float NdotV, float roughness){
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness){
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = geometry_schlick_GGX(NdotV, roughness);
  float ggx1 = geometry_schlick_GGX(NdotL, roughness);

  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnel_schlick(float cos_theta, vec3 F0){
  return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnel_schlick_roughness(float cos_theta, vec3 F0, float roughness){
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cos_theta, 5.0);
}
// ----------------------------------------------------------------------------
const float FogDensity = 0.1f;

float calcule_fog(){
  float dist = 0;
  float fogFactor = 0;

  //range based
  dist = length(o_position.xyz) - 45.0f;
  dist = (dist < 0.0f)? 0 : dist;

  //exponential fog
  fogFactor = 1.0 /exp(dist * FogDensity);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  return fogFactor;
}
// ----------------------------------------------------------------------------

void main(void){
  // material properties
  vec4 diffusive = (u_colored)? u_color : texture(u_albedo, o_uv).rgba;
//  vec3 albedo = pow(diffusive.rgb, vec3(2.2));
  vec3 albedo = pow(diffusive.rgb, vec3(2.2));
  float metallic = (u_metallized)? u_metallic_value : texture(u_metallic, o_uv).r;
  float roughness = (u_roughed)? u_roughness_value : texture(u_roughness, o_uv).r;
  float ao = texture(u_ao, o_uv).r;
//  vec4 diffusive = mix(texture(u_albedo, o_uv).rgba, u_color, u_colored);
//  vec3 albedo = pow(diffusive.rgb, vec3(2.2));
//  float metallic = mix(texture(u_metallic, o_uv).r, u_metallized, u_metallic_value);
//  float roughness = mix(texture(u_roughness, o_uv).r, u_roughness_value, u_roughed);
//  float ao = texture(u_ao, o_uv).r;

  // input lighting data
  vec3 N = get_normal_from_map();
  vec3 R = reflect(-o_V, N);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);
  float maxDotNV = max(dot(N, o_V), 0.0);

  for(int i = 0; i < o_light_size; ++i){
    // Cook-Torrance BRDF
    float NDF = distribution_GGX(N, o_H[i], roughness);
    float G = geometry_smith(N, o_V, o_L[i], roughness);
    vec3 F = fresnel_schlick(max(dot(o_H[i], o_V), 0.0), F0);

    vec3 nominator = NDF * G * F;
    // 0.001 to prevent divide by zero.
    float maxDotNL = max(dot(N, o_L[i]), 0.0);
    float denominator = 4 * maxDotNV * maxDotNL + 0.001;
    vec3 specular = nominator / denominator;

    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - F.
    vec3 kD = vec3(1.0) - F;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = maxDotNL;

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * o_radiance[i] * NdotL;
    // note that we already multiplied the BRDF by the Fresnel (F) so we won't multiply by F again
  }

  // ambient lighting (we now use IBL as the ambient term)
  vec3 F = fresnel_schlick_roughness(maxDotNV, F0, roughness);

  vec3 kD = (1.0 - F) * (1.0 - metallic);

  vec3 irradiance = texture(u_irradiance, N).rgb;
  vec3 diffuse = irradiance * albedo;

  // sample both the pre-filter map and the BRDF lut and combine them together as per the
  // Split-Sum approximation to get the IBL specular part.
  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = textureLod(u_prefilter, R, roughness * o_light_size).rgb;
  vec2 brdf = texture(u_brdfLUT, vec2(maxDotNV, roughness)).rg;
  vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * ao;

  vec3 color = ambient + Lo;

  // HDR tonemapping
  color = color / (color + vec3(1.0));
  // gamma correct
  color = pow(color, vec3(1.0/2.2));

  if(u_fog)
    diffusive.a = calcule_fog();

  frag_color = vec4(color, diffusive.a);
}
