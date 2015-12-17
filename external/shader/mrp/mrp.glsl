vec3 getDirectionToLight(inout float evergyFactor, in Sphere sphere, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  evergyFactor *= 1.f;
  return normalize(sphere.origin-surface.position);
}

vec3 getDirectionToLight(inout float evergyFactor, in Tube tube, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  evergyFactor *= 1.f;
  return normalize(tube.origin-surface.position);
}