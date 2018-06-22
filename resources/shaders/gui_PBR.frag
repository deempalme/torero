#version 420 core
#extension GL_EXT_shader_texture_lod: enable
// Physically based rendering for the GUI

in vec2 o_uv;
in vec3 o_position;
in vec3 o_position_2;
in mat3 o_TBN;
in vec3 o_color;

// 2D textures
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
// Emission effect
uniform float u_emitting;
uniform sampler2D u_emission;
// metallic effect
uniform sampler2D u_metallic;
// roughness reflectiviness
uniform sampler2D u_roughness;
// ambient occlusion texture
uniform sampler2D u_ao;
// irradiance maps
uniform samplerCube u_irradiance;
uniform samplerCube u_prefilter;
uniform sampler2D u_brdfLUT;
// lights
uniform vec3 u_lights[4];
uniform vec3 u_light;
uniform vec3 u_light_color;
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
  const vec3 tangentNormal = texture(u_normal, o_uv).xyz * 2.0 - 1.0;
  return normalize(o_TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
vec3 SRGBtoLINEAR(const vec3 srgbIn)
{
  return pow(srgbIn, vec3(2.2));
}
float SRGBtoLINEAR(const float srgbIn)
{
  return pow(srgbIn, 2.2);
}
// ----------------------------------------------------------------------------
float energize(float value){
  return energy * pow(value, shininess);
}
// ----------------------------------------------------------------------------
float DistributionGGX(const vec3 N, const vec3 H, const float roughness)
{
  const float a = roughness * roughness;
  const float a2 = a * a;
  const float NdotH = max(dot(N, H), 0.0);
  const float NdotH2 = NdotH * NdotH;

  const float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(const float NdotV, const float roughness)
{
  const float r = (roughness + 1.0);
  const float k = (r*r) / 8.0;

  const float nom   = NdotV;
  const float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(const vec3 N, const vec3 V, const vec3 L, const float roughness)
{
  const float NdotV = max(dot(N, V), 0.0);
  const float NdotL = max(dot(N, L), 0.0);
  const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  const float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(const float cosTheta, const vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(const float cosTheta, const vec3 F0, const float roughness)
{
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
  // material properties
  const vec4 full_albedo = texture(u_albedo, o_uv).rgba;
  const vec3 albedo = SRGBtoLINEAR(full_albedo.rgb);
  const float alpha = full_albedo.a;
  const float metallic = SRGBtoLINEAR(texture(u_metallic, o_uv).r);
  const float roughness = SRGBtoLINEAR(texture(u_roughness, o_uv).r);
  const float ao = SRGBtoLINEAR(texture(u_ao, o_uv).r);

  // input lighting data
  const vec3 N = getNormalFromMap();
  const vec3 V = normalize(u_camera - o_position);
//  const vec3 V = normalize(normalize(u_camera) - normalize(o_position_2));
  const vec3 R = reflect(-V, N);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);

  // Calculating point lights ---------------------------------------------------------------
//  vec3 light_color = vec3(0.46, 0.745, 1.0);

  /*
  for(int i = 0; i < 4; ++i){
    // calculate per-light radiance
    vec3 L = normalize(u_lights[i] - o_position);
//    vec3 L = normalize(u_lights[i]);
    vec3 H = normalize(V + L);
    float distance = length(u_lights[i] - o_position);
//    float distance = length(u_lights[i]);
    float attenuation = 0.2 / (distance * distance);
//    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    vec3 radiance = light_color * attenuation;

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
  */
  /*
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
  */

  // ambient lighting (we now use IBL as the ambient term)
  const vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

  const vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;

  const vec3 irradiance = SRGBtoLINEAR(texture(u_irradiance, N).rgb);
  const vec3 diffuse    = irradiance * albedo;

  // sample both the pre-filter map and the BRDF lut and combine them together as per the
  // Split-Sum approximation to get the IBL specular part.
  const float MAX_REFLECTION_LOD = 4.0;
  const vec3 prefilteredColor = SRGBtoLINEAR(textureLod(u_prefilter, R,
                                                        roughness * MAX_REFLECTION_LOD).rgb);
  const vec2 brdf  = SRGBtoLINEAR(texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rgb).rg;
  const vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

  const vec3 ambient = (kD * diffuse + specular) * ao;

  vec3 color = ambient + Lo;

  // HDR tonemapping
//  color = color / (color + vec3(1.0));
  // gamma correct
  color = pow(color, vec3(1.0/2.2));
  color = mix(color, color + texture(u_emission, o_uv).rgb * o_color, u_emitting);

  frag_color = vec4(color, alpha);
}
