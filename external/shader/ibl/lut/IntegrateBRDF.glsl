vec2 IntegrateBRDF(float Roughness, float NoV)
{
  vec3 V;
  V.x = sqrt(1.0f - NoV * NoV); // sin
  V.y = 0;
  V.z = NoV;
  
  // cos
  float A = 0;
  float B = 0;
  
  const uint NumSamples = 1024;
  for( uint i = 0; i < NumSamples; i++ )
  {
    vec2 Xi = Hammersley(i, NumSamples);
    vec3 H = ImportanceSampleGGX(Xi, Roughness, N);
    vec3 L = 2 * dot(V, H) * H - V;
    
    float NoL = saturate(L.z);
    float NoH = saturate(H.z);
    float VoH = saturate(dot(V, H));
    
    if( NoL > 0 )
    {
      float G = G_Smith(Roughness, NoV, NoL);
      float G_Vis = G * VoH / (NoH * NoV);
      float Fc = pow(1 - VoH, 5);
      A += (1 - Fc) * G_Vis;
      B += Fc * G_Vis;
    }
  }
  
  return vec2(A, B) / NumSamples;
}
