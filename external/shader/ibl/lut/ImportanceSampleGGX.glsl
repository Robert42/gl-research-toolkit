vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N)
{
  float a = Roughness * Roughness;
  float Phi = 2 * pi * Xi.x;
  float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
  float SinTheta = sqrt( 1 - CosTheta * CosTheta );
  
  vec3 H;
  H.x = SinTheta * cos( Phi );
  H.y = SinTheta * sin( Phi );
  H.z = CosTheta;
  
  vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
  vec3 TangentX = normalize( cross( UpVector , N ) );
  vec3 TangentY = cross( N, TangentX );
  // Tangent to world space
  return TangentX * H.x + TangentY * H.y + N * H.z;
}
