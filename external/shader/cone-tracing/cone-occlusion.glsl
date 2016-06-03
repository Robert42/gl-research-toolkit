#ifndef CONETRACING_CONE_OCCLUSION_GLSL
#define CONETRACING_CONE_OCCLUSION_GLSL

float coneOcclusionHeuristic(in Cone cone, float t, float distanceToSurface, float strength=1.f)
{
  return mix(1.f, distanceToSurface / (cone.tan_half_angle * t), clamp(strength, 0.f, 1.f));
}

#endif // CONETRACING_CONE_OCCLUSION_GLSL