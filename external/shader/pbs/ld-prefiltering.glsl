// Based on Listing 19
// Pre-filtered importance sampling function.

vec3 integrateCubeLDOnly(
  in vec3 V,
  in vec3 N,
  in float roughness)
{
  vec3 accBrdf = 0;
  float accBrdfWeight = 0;
  for(uint i =0; i<sampleCount; ++i)
  {
    float2 eta = getSample (i , sampleCount ) ;
    vec3 L ;
    vec3 H ;
    importanceSampleGGXDir(eta, V, N, roughness, H, L);
    float NdotL = dot(N, L);
    if(NdotL>0)
    {
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
      float mipLevel = clamp(0.5 * log2(omegaS / omegaP),0 ,mipCount);
      vec4 Li = IBLCube.SampleLevel(IBLSampler, L, mipLevel);
      
      accBrdf += Li.rgb * NdotL;
      accBrdfWeight += NdotL;
    }
  }
  return accBrdf * (1.0f / accBrdfWeight);
}
