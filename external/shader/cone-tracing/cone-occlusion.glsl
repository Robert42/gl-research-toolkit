#ifndef CONETRACING_CONE_OCCLUSION_GLSL
#define CONETRACING_CONE_OCCLUSION_GLSL

float coneOcclusionHeuristic(in Cone cone, float t, float distanceToSurface)
{
  return distanceToSurface / (cone.tan_half_angle * t);
}

#endif // CONETRACING_CONE_OCCLUSION_GLSL