// based on Listing 18
// Pre-integrated DFG function for both specular GGX and diffuse Disney BDRF

void importanceSampleCosDir(
  in vec2 u,
  in vec3 N,
  out(vec3) L,
  out(float) NdotL,
  out(float) pdf)
{
  // Local referencial
  vec3 upVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
  vec3 tangentX = normalize(cross(upVector, N));
  vec3 tangentY = cross(N, tangentX);
  float u1 = u.x;
  float u2 = u.y;
  float r = sqrt(u1);
  float phi = u2 * pi * 2;
  L = vec3(r * cos(phi), r*sin(phi), sqrt(max(0.0f, 1.0f - u1)));
  L = normalize(tangentX * L.y + tangentY * L.x + N * L.z);
  NdotL = dot(L, N);
  pdf = NdotL * inv_pi;
}

vec3 integrateDFGOnly(
  in vec3 V,
  in vec3 N,
  in float roughness)
{
  float NdotV = saturate ( dot (N , V ) ) ;
  vec3 acc = vec3(0);
  float accWeight = 0;
  
  // Compute pre-integration
  uint sampleCount = 1024;
  for(uint i=0; i < sampleCount; ++i)
  {
    vec2 u = getSample(i, sampleCount);
    vec3 L = vec3(0);
    float NdotH = 0;
    float LdotH = 0;
    float G = 0;
    // See [ Karis13 ] for implementation
    importanceSampleGGX_G(u, V, N, roughness, NdotH, LdotH, L, G);
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
    u = fract(u + vec2(0.5));
    float pdf ;
    // The pdf is not use because it cancel with other terms
    // (The 1/ PI from diffuse BRDF and the NdotL from Lambert ’ s law).
    importanceSampleCosDir(u, N, L, NdotL, pdf);
    if(NdotL >0)
    {
      float LdotH = saturate(dot(L, normalize(V + L)));
      float NdotV = saturate(dot(N, V));
      acc.z += Fr_DisneyDiffuse(NdotV, NdotL, LdotH, sqrt(roughness));
    }
    accWeight += 1.0;
  }
  return acc * (1.0f / accWeight);
}
