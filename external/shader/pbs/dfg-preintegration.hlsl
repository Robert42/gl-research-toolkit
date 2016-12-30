// based on Listing 18
// Pre-integrated DFG function for both specular GGX and diffuse Disney BDRF

void importanceSampleCosDir(
  in float2 u,
  in float3 N,
  out float3 L,
  out float NdotL,
  out float pdf)
{
  // Local referencial
  float3 upVector = abs(N.z) < 0.999 ? float3 (0,0,1) : float3 (1,0,0) ;
  float3 tangentX = normalize(cross(upVector, N));
  float3 tangentY = cross(N, tangentX);
  float u1 = u.x;
  float u2 = u.y;
  float r = sqrt(u1);
  float phi = u2 * FB_PI * 2;
  L = float3(r * cos(phi), r*sin(phi), sqrt(max(0.0f, 1.0f - u1)));
  L = normalize(tangentX * L.y + tangentY * L.x + N * L.z);
  NdotL = dot(L, N);
  pdf = NdotL * FB_INV_PI;
}

float4 integrateDFGOnly(
  in float3 V,
  in float3 N,
  in float roughness)
{
  float NdotV = saturate ( dot (N , V ) ) ;
  float4 acc = 0;
  float accWeight = 0;
  
  // Compute pre-integration
  Referential referential = createReferential(N);
  for(uint i=0; i < sampleCount; ++i)
  {
    float2 u = getSample(i, sampleCount);
    float3 L = 0;
    float NdotH = 0;
    float LdotH = 0;
    float G = 0;
    // See [ Karis13 ] for implementation
    importanceSampleGGX_G(u, V, N, referential, roughness, NdotH, LdotH, L, G);
    // specular GGX DFG preIntegration
    float NdotL = saturate(dot(N, L));
    if(NdotL>0 && G>0.0)
    {
      float GVis = G * LdotH / (NdotH * NdotV) ;
      float Fc = pow (1 - LdotH , 5.f);
      acc.x += (1 - Fc) * GVis;
      acc . y += Fc * GVis;
    }
    // diffuse DisneypreIntegration
    u = frac(u + 0.5);
    float pdf ;
    // The pdf is not use because it cancel with other terms
    // (The 1/ PI from diffuse BRDF and the NdotL from Lambert ’ s law).
    importanceSampleCosDir(u, N, L, NdotL, pdf)
    if(NdotL >0)
    {
      float LdotH = saturate(dot(L, normalize(V + L));
      float NdotV = saturate(dot(N, V));
      acc.z += Fr_DisneyDiffuse(NdotV, NdotL, LdotH, sqrt(roughness));
    }
    accWeight += 1.0;
  }
  return acc * (1.0f / accWeight);
}
