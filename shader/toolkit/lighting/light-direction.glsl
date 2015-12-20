
vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Disk disk, in SurfaceData surface)
{
  // TODO
  specularEnergyFactor = 1.f;
  light_distance = length(disk.origin-surface.position);
  return (disk.origin-surface.position) / light_distance;
}

vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Rect rect, in SurfaceData surface)
{
  vec2 half_size = vec2(rect.half_width, rect.half_height);
  
  vec3 reflection_direction = get_mrp_reflection_direction(surface);
  rect.origin -= surface.position;
  
  float t1 =_closestPointToLine_unclamped(rect.origin,
                                          rect.tangent1*rect.half_width,
                                          sq(rect.half_width),
                                          reflection_direction);
  float t2 =_closestPointToLine_unclamped(rect.origin,
                                          rect.tangent2*rect.half_height,
                                          sq(rect.half_height),
                                          reflection_direction);

  vec2 t = vec2(t1, t2);
  
  t = clamp(t, vec2(-1), vec2(1));
  
  t = clamp(t, vec2(0), vec2(1));// FIXME: remove this
  t.y = 0; // FIXME: remove this
  
  vec3 nearest_point = map_point_from_rect_plane(rect, t * half_size);
  
  light_distance = length(nearest_point);
  float radius = mix(rect.half_width, rect.half_height, 0.5f);
  
  vec3 l = nearest_point / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(radius, light_distance, surface);
  
  Ray ray;
  ray.origin = surface.position;
  ray.direction = reflection_direction;
  PRINT_VALUE(nearest_point+surface.position, true);
  PRINT_VALUE(ray, true);
  vec3 i;
  rect.origin += surface.position;
  if(intersection_point(plane_from_rect(rect), ray, i))
    PRINT_VALUE(i, true);
  
  return l;
}