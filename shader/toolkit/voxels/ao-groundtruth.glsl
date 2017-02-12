#ifdef COMPUTE_SHADER
#ifdef AO_GROUNDTRUTH_SAMPLES
#undef AO_GROUNDTRUTH_SAMPLES
#endif
#define AO_GROUNDTRUTH_SAMPLES 0

#endif


#ifndef AO_GROUNDTRUTH_SAMPLES
#error AO_GROUNDTRUTH_SAMPLES undefined
#endif


#if AO_GROUNDTRUTH_SAMPLES > 0

#ifndef AO_GROUNDTRUTH_CONE_HALF_TAN_ANGLE
#error AO_GROUNDTRUTH_CONE_HALF_TAN_ANGLE undefined
#endif


#undef AO_FALLBACK_NONE
#define AO_FALLBACK_NONE 0
#undef AO_FALLBACK_LINEAR
#define AO_FALLBACK_LINEAR 0
#undef AO_FALLBACK_CLAMPED
#define AO_FALLBACK_CLAMPED 0
#undef AO_IGNORE_FALLBACK_SDF
#define AO_IGNORE_FALLBACK_SDF 1
#undef AO_FALLBACK_SDF_ONLY
#define AO_FALLBACK_SDF_ONLY 0
#undef NO_BVH
#define NO_BVH 1



#include <random.glsl>
#include <pbs/importance_sample_cos_dir.glsl>




vec3 groundtruth_cone_direction(out bool contribution, uint i, vec3 N)
{
  float NdotL;
  float pdf;
  vec3 L;
  vec2 u = getSample(i, AO_GROUNDTRUTH_SAMPLES);
  importanceSampleCosDir(u, N, L, NdotL, pdf);

  contribution = acos(NdotL) + atan(AO_GROUNDTRUTH_CONE_HALF_TAN_ANGLE) < radians(90);

  return L;
}


#endif
