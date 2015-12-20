
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
  
  vec3 half_axis1 = rect.tangent1*rect.half_width;
  vec3 half_axis2 = rect.tangent2*rect.half_height;
  vec3 axis1 = half_axis1*2.f;
  vec3 axis2 = half_axis2*2.f;
  vec3 origin = rect.origin - surface.position - half_axis1 - half_axis2;
  
  float t1 =_closestPointToLine_unclamped(origin,
                                          axis1,
                                          sq(rect.half_width*2.f),
                                          reflection_direction);
  float t2 =_closestPointToLine_unclamped(origin,
                                          axis2,
                                          sq(rect.half_height*2.f),
                                          reflection_direction);

  vec2 t = vec2(t1, t2);
  t = abs(t);
  
  PRINT_VALUE(t);
  
  t = clamp(t, vec2(0), vec2(1));
  
  vec3 nearest_point = origin + t.x*axis1 + t.y*axis2;
  
  light_distance = length(nearest_point);
  float radius = mix(rect.half_width, rect.half_height, 0.5f);
  
  vec3 l = nearest_point / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(radius, light_distance, surface);
  
  Ray ray;
  ray.origin = surface.position;
  ray.direction = reflection_direction;
  PRINT_VALUE(nearest_point+surface.position, true);
  PRINT_VALUE(ray, true);
  /*
  vec3 i;
  if(intersection_point(plane_from_rect(rect), ray, i))
    PRINT_VALUE(i, true);
  */
  return l;
}