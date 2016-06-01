float coneOcclusionHeuristic(in Cone cone, float t, float distanceToSurface)
{
  return distanceToSurface / (cone.tan_half_angle * t);
}