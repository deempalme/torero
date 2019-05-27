#version 420 core
// GLSL fragment shader for bidirectional reflective distribution function

in vec2 o_uv;

layout (location = 0) out vec4 o_frag_color;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
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
vec3 importance_sample_GGX(vec2 Xi, vec3 N, float roughness)
{
  float a = roughness * roughness;

  float phi = 2.0 * PI * Xi.x;
  float cos_theta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
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
float geometry_schlick_GGX(float NoV, float roughness){
  // note that we use a different k for IBL
  float a = roughness;
  float k = (a * a) / 2.0;

  float nom   = NoV;
  float denom = NoV * (1.0 - k) + k;

  return nom / denom;
}
// ----------------------------------------------------------------------------
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness){
  float NoV = max(dot(N, V), 0.0);
  float NoL = max(dot(N, L), 0.0);
  float ggx2 = geometry_schlick_GGX(NoV, roughness);
  float ggx1 = geometry_schlick_GGX(NoL, roughness);

  return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec2 integrate_BRDF(float NoV, float roughness){
  vec3 V;
  V.x = sqrt(1.0 - NoV * NoV);
  V.y = 0.0;
  V.z = NoV;

  float A = 0.0;
  float B = 0.0;

  vec3 N = vec3(0.0, 0.0, 1.0);

  const uint SAMPLE_COUNT = 1024u;
  for(uint i = 0u; i < SAMPLE_COUNT; ++i){
    // generates a sample vector that's biased towards the
    // preferred alignment direction (importance sampling).
    vec2 Xi = hammersley(i, SAMPLE_COUNT);
    vec3 H = importance_sample_GGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);

    float NoL = max(L.z, 0.0);
    float NoH = max(H.z, 0.0);
    float VoH = max(dot(V, H), 0.0);

    if(NoL > 0.0){
      float G = geometry_smith(N, V, L, roughness);
      float G_Vis = (G * VoH) / (NoH * NoV);
      float Fc = pow(1.0 - VoH, 5.0);

      A += (1.0 - Fc) * G_Vis;
      B += Fc * G_Vis;
    }
  }
  A /= float(SAMPLE_COUNT);
  B /= float(SAMPLE_COUNT);
  return vec2(A, B);
}

// ----------------------------------------------------------------------------
void main(){
  o_frag_color = vec4(0.0, integrate_BRDF(o_uv.x, o_uv.y).yx, 1.0);
}
