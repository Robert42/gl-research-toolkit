vec3 getDirectionToLight(in Disk disk, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(disk.origin-surface.position);
}

vec3 getDirectionToLight(in Sphere sphere, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(sphere.origin-surface.position);
}

vec3 getDirectionToLight(in Rect rect, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(rect.origin-surface.position);
}

vec3 getDirectionToLight(in Tube tube, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  return normalize(tube.origin-surface.position);
}