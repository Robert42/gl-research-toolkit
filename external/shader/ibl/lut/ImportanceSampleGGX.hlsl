float3 ImportanceSampleGGX( float2 Xi, float Roughness , float3 N )
{
  float a = Roughness * Roughness;
  float Phi = 2 * PI * Xi.x;
  float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
  float SinTheta = sqrt( 1 - CosTheta * CosTheta );
  
  float3 H;
  H.x = SinTheta * cos( Phi );
  H.y = SinTheta * sin( Phi );
  H.z = CosTheta;
  
  float3 UpVector = abs(N.z) < 0.999 ? float3(0,0,1) : float3(1,0,0);
  float3 TangentX = normalize( cross( UpVector , N ) );
  float3 TangentY = cross( N, TangentX );
  // Tangent to world space
  return TangentX * H.x + TangentY * H.y + N * H.z;
}
