#ifndef _IMPORTANCE_SAMPLE_DIR_
#define _IMPORTANCE_SAMPLE_DIR_


// Based on listing A.2 
void importanceSampleCosDir(
    in vec2 u,
    in vec3 N,
    out vec3 L,
    out float NdotL,
    out float pdf)
{
  // Local referencial
  vec3 upVector = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1 ,0 ,0);
  vec3 tangentX = normalize(cross(upVector, N));
  vec3 tangentY = cross(N, tangentX);
  float u1 = u.x;
  float u2 = u.y;
  float r = sqrt(u1);
  float phi = u2 * pi * 2;
  L = vec3(r * cos(phi), r * sin(phi), sqrt(max(0.0f, 1.0f - u1)));
  L = normalize (tangentX * L.y + tangentY * L.x + N * L.z);
  NdotL = dot(L, N);
  pdf = NdotL * inv_pi;
}

#endif // _IMPORTANCE_SAMPLE_DIR_
