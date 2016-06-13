#ifndef CONETRACING_CONE_OCCLUSION_GLSL
#define CONETRACING_CONE_OCCLUSION_GLSL

float coneOcclusionHeuristic(float cone_radius, float distanceToSurface)
{
  return distanceToSurface / cone_radius;
}

#endif // CONETRACING_CONE_OCCLUSION_GLSL