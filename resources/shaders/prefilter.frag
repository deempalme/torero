#version 420 core
// GLSL fragment shader for prefiltering in PBR

in vec3 f_position;
in vec3 f_up;
in vec3 f_right;

layout (location = 0) out vec4 o_frag_color;

uniform samplerCube u_skybox;
uniform float u_roughness;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
float distribution_GGX(vec3 N, vec3 H, float roughness){
  float a = roughness * roughness;
  float a2 = a * a;
  float NoH = max(dot(N, H), 0.0);
  float NoH2 = NoH * NoH;

  float nom   = a2;
  float denom = (NoH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_hammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float radical_inverse_VdC(uint bits){
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 hammersley(uint i, uint N){
  return vec2(float(i)/float(N), radical_inverse_VdC(i));
}
// ----------------------------------------------------------------------------
vec3 importance_sample_GGX(vec2 Xi, vec3 N, float roughness){
  float a = roughness*roughness;

  float phi = 2.0 * PI * Xi.x;
  float cos_theta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
  float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

  // from spherical coordinates to cartesian coordinates - halfway vector
  vec3 H;
  H.x = cos(phi) * sin_theta;
  H.y = sin(phi) * sin_theta;
  H.z = cos_theta;

  // from tangent-space H vector to world-space sample vector
  vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent   = normalize(cross(up, N));
  vec3 bitangent = cross(N, tangent);

  vec3 sample_vec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sample_vec);
}
// ----------------------------------------------------------------------------
void main()
{
  // make the simplyfying assumption that V equals R equals the normal
  vec3 R = f_position;
  vec3 V = R;

  const uint SAMPLE_COUNT = 1024u;
  vec3 prefiltered_color = vec3(0.0);
  float total_weight = 0.0;

  for(uint i = 0u; i < SAMPLE_COUNT; ++i){
    // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
    vec2 Xi = hammersley(i, SAMPLE_COUNT);
    vec3 H  = importance_sample_GGX(Xi, f_position, u_roughness);
    vec3 L  = normalize(2.0 * dot(V, H) * H - V);

    float NoL = max(dot(f_position, L), 0.0);

    if(NoL > 0.0){
      // sample from the environment's mip level based on roughness/pdf
      float D   = distribution_GGX(f_position, H, u_roughness);
      float NoH = max(dot(f_position, H), 0.0);
      float HoV = max(dot(H, V), 0.0);
      float pdf = D * NoH / (4.0 * HoV) + 0.0001;

      float resolution = 512.0; // resolution of source cubemap (per face)
      float sa_texel   = 4.0 * PI / (6.0 * resolution * resolution);
      float sa_sample  = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

      float mip_level = (u_roughness == 0.0) ? 0.0 : 0.5 * log2(sa_sample / sa_texel);

      prefiltered_color += textureLod(u_skybox, L, mip_level).rgb * NoL;
      total_weight      += NoL;
    }
  }

  prefiltered_color = prefiltered_color / total_weight;

  o_frag_color = vec4(prefiltered_color, 1.0);
}
