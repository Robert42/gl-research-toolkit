// Pre-filtered importance sampling function.

#include <ibl/ggx_importance/ImportanceSampleGGX.glsl>


vec4 sample_environment(vec3 view, float mipLevel=0);
vec2 getSample(uint i, uint sampleCount);

void importanceSampleGGX_(vec2 u, vec3 V, vec3 N, float roughness, out float NdotH, out float LdotH, out vec3 H, out vec3 L, out float G)
{
  L = ImportanceSampleGGX(u, roughness, N);

  H = normalize(V + L);
  NdotH  = saturate(dot(N, H));
  LdotH  = saturate(dot(L, H));

  G = D_GGX(NdotH, roughness);
}

void importanceSampleGGXDir(vec2 u, vec3 V, vec3 N, float roughness, out vec3 H, out vec3 L)
{
  float NdotH, LdotH, G;
  
  importanceSampleGGX_(u, V, N, roughness, NdotH, LdotH, H, L, G);
}

// Based on Listing 19
vec3 integrateCubeLDOnly(
  in vec3 V,
  in vec3 N,
  in float roughness,
  in int sampleCount=1024)
{
  vec3 accBrdf = vec3(0);
  float accBrdfWeight = 0;
  for(uint i =0; i<sampleCount; ++i)
  {
    vec2 eta = getSample (i , sampleCount ) ;
    vec3 L ;
    vec3 H ;
    importanceSampleGGXDir(eta, V, N, roughness, H, L);
    float NdotL = dot(N, L);
    if(NdotL>0)
    {
#if 0
      // Use pre - filtered importance sampling ( i . e use lower mipmap
      // level for fetching sample with low probability in order
      // to reduce the variance ) .
      // ( Reference : GPU Gem3 )
      //
      // Since we pre - integrate the result for normal direction ,
      // N == V and then NdotH == LdotH . This is why the BRDF pdf
      // can be simplifed from :
      //
      pdf = D_GGX_Divide_Pi(NdotH, roughness) * NdotH / (4*LdotH);
      // to
      //
      pdf = D_GGX_Divide_Pi(NdotH, roughness) / 4;
      //
      // The mipmap level is clamped to something lower than 8 x8
      // in order to avoid cubemap filtering issues
      //
      // - OmegaS : Solid angle associated to a sample
      // - OmegaP : Solid angle associated to a pixel of the cubemap
      float NdotH = saturate(dot(N, H));
      float LdotH = saturate(dot(L, H));
      float pdf = D_GGX_Divide_Pi(NdotH, roughness) * NdotH / (4* LdotH);
      float omegaS = 1.0 / (sampleCount * pdf);
      float omegaP = 4.0 * FB_PI / (6.0 * width * width);
      float mipLevel = clamp(0.5 * log2(omegaS / omegaP), 0, mipCount);
#else
      float mipLevel = 0;
#endif
      vec4 Li = sample_environment(L, mipLevel);
      
      accBrdf += Li.rgb * NdotL;
      accBrdfWeight += NdotL;
    }
  }
  return accBrdf * (1.0f / accBrdfWeight);
}

void importanceSampleCosDir(
    in vec2 u,
    in vec3 N,
    out vec3 L,
    out float NdotL,
    out float pdf);

// Based on Listing 20
vec4 integrateDiffuseCube(in vec3 N, in int sampleCount=1024)
{
  vec3 accBrdf = vec3(0);
  for(uint i=0; i<sampleCount; ++i)
  {
    vec2 eta = getSample(i, sampleCount);
    vec3 L;
    float NdotL;
    float pdf;
    // see reference code in appendix
    importanceSampleCosDir(eta, N, L, NdotL, pdf);
    if(NdotL >0)
      accBrdf += sample_environment(L).rgb;
  }
  return vec4(accBrdf * (1.0f / sampleCount), 1.0f);
}

#include "importance_sample_cos_dir.glsl"
