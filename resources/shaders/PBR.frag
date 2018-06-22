#version 420 core
#extension GL_EXT_shader_texture_lod: enable
// GLSL fragment shader with physically based rendering and fog

in vec2 o_uv;
in vec3 o_position;
in mat3 o_TBN;

uniform bool u_pbr;
uniform bool u_fog;
// 2D textures
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
// Emission effect
uniform int u_emitting;
uniform sampler2D u_emission;
// metallic effect
uniform int u_metallized;
uniform sampler2D u_metallic;
uniform float u_metallic_value;
// roughness reflectiviness
uniform int u_roughed;
uniform sampler2D u_roughness;
uniform float u_roughness_value;
// ambient occlusion texture
uniform sampler2D u_ao;
// use color instead of texture
uniform int u_colored;
uniform vec4 u_color;
// irradiance maps
uniform samplerCube u_irradiance;
uniform samplerCube u_prefilter;
uniform sampler2D u_brdfLUT;
// lights
uniform vec3 u_light[4];
uniform vec3 u_light_color[4];
uniform vec3 u_sun;
uniform vec3 u_sun_color;
// Camera position
uniform vec3 u_camera;

//output color
out vec4 frag_color;

const float PI = 3.14159265359;
const float shininess = 16.0;
const float energy = (2.0 + shininess) / (2.0 * PI);
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
  vec3 tangentNormal = texture(u_normal, o_uv).xyz * 2.0 - 1.0;
  return normalize(o_TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
vec3 SRGBtoLINEAR(vec3 srgbIn)
{
  vec3 linOut = pow(srgbIn, vec3(2.2));
  return vec3(linOut);;
}
// ----------------------------------------------------------------------------
float energize(float value){
  return energy * pow(value, shininess);
//  return value;
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float nom   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
  // material properties
  vec3 albedo = mix(SRGBtoLINEAR(texture(u_albedo, o_uv).rgb), u_color.rgb, u_colored);
  float alpha = mix(texture(u_albedo, o_uv).a, u_color.a, u_colored);
  float metallic = mix(SRGBtoLINEAR(texture(u_metallic, o_uv).rgb).r, u_metallic_value, u_metallized);
  float roughness = mix(SRGBtoLINEAR(texture(u_roughness, o_uv).rgb).r, u_roughness_value, u_roughed);
  float ao = SRGBtoLINEAR(texture(u_ao, o_uv).rgb).r;

  // input lighting data
  vec3 N = getNormalFromMap();
  vec3 V = normalize(u_camera - o_position);
  vec3 R = reflect(-V, N);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);

  // Calculating point lights ---------------------------------------------------------------
  for(int i = 0; i < 4; ++i){
    // calculate per-light radiance
//    vec3 L = normalize(u_light[i] - o_position);
    vec3 L = normalize(u_light[i]);
    vec3 H = normalize(V + L);
//    float distance = length(u_light[i] - o_position);
    float distance = length(u_light[i]);
//    float attenuation = 1.0 / (distance * distance);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    vec3 radiance = u_light_color[i] * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator    = NDF * G * F;
    // 0.001 to prevent divide by zero.
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    // note that we already multiplied the BRDF by the Fresnel (kS)
    // so we won't multiply by kS again
  }
  // Calculating directional light ----------------------------------------------------------
  // calculate per-light radiance
  vec3 L = normalize(u_sun);
  vec3 H = normalize(V + L);
  vec3 radiance = u_sun_color;

  // Cook-Torrance BRDF
  float NDF = DistributionGGX(N, H, roughness);
  float G   = GeometrySmith(N, V, L, roughness);
  vec3 F_s  = fresnelSchlick(max(dot(H, V), 0.0), F0);

  vec3 nominator    = NDF * G * F_s;
  // 0.001 to prevent divide by zero.
  float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  vec3 specular_s = nominator / denominator;

  // kS is equal to Fresnel
  vec3 kS_s = F_s;
  // for energy conservation, the diffuse and specular light can't
  // be above 1.0 (unless the surface emits light); to preserve this
  // relationship the diffuse component (kD) should equal 1.0 - kS.
  vec3 kD_s = vec3(1.0) - kS_s;
  // multiply kD by the inverse metalness such that only non-metals
  // have diffuse lighting, or a linear blend if partly metal (pure metals
  // have no diffuse light).
  kD_s *= 1.0 - metallic;

  // scale light by NdotL
  float NdotL = max(dot(N, L), 0.0);

  // add to outgoing radiance Lo
  Lo += (kD_s * albedo / PI + specular_s) * radiance * NdotL;
  // note that we already multiplied the BRDF by the Fresnel (kS)
  // so we won't multiply by kS again
  // End alculating directional light -------------------------------------------------------

  // ambient lighting (we now use IBL as the ambient term)
  vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;

  vec3 irradiance = SRGBtoLINEAR(texture(u_irradiance, N).rgb);
  vec3 diffuse    = irradiance * albedo;

  // sample both the pre-filter map and the BRDF lut and combine them together as per the
  // Split-Sum approximation to get the IBL specular part.
  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = SRGBtoLINEAR(textureLod(u_prefilter, R, roughness * MAX_REFLECTION_LOD).rgb);
  vec2 brdf  = SRGBtoLINEAR(texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rgb).rg;
  vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * ao;

  vec3 color = ambient + Lo;

  // HDR tonemapping
//  color = color / (color + vec3(1.0));
  // gamma correct
  color = pow(color, vec3(1.0/2.2));
  color = mix(color, mix(color, color * 0.2 + albedo * 0.8, texture(u_emission, o_uv).r), u_emitting);

  frag_color = vec4(color, alpha);
}
