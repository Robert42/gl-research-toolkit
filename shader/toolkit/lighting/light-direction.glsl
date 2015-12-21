
vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Disk disk, in SurfaceData surface, in vec3 dominant_reflection_direction)
{
  // TODO
  specularEnergyFactor = 1.f;
  light_distance = length(disk.origin-surface.position);
  return (disk.origin-surface.position) / light_distance;
}

vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Rect rect, in SurfaceData surface, in vec3 dominant_reflection_direction)
{
  vec2 half_size = vec2(rect.half_width, rect.half_height);
  
  vec3 reflection_direction = dominant_reflection_direction;
  
  float width = rect.half_width*2.f;
  float height = rect.half_height*2.f;
  
  vec3 half_axis1 = rect.tangent1*rect.half_width;
  vec3 half_axis2 = rect.tangent2*rect.half_height;
  vec3 axis1 = half_axis1*2.f;
  vec3 axis2 = half_axis2*2.f;
  vec3 origin = rect.origin - surface.position - half_axis1 - half_axis2;
  
  float t1 =_closestPointToLine_unclamped(origin,
                                          axis1,
                                          sq(width),
                                          reflection_direction);
  float t2 =_closestPointToLine_unclamped(origin,
                                          axis2,
                                          sq(height),
                                          reflection_direction);

  vec2 t_lines = vec2(t1, t2);
  t_lines = abs(t_lines);
  t_lines = clamp(t_lines, vec2(0), vec2(1));
  
  // try hiding the artifacts by using a fallback mode if the reflection ray is more parallel to the axis
  vec2 t_nearest_edge = vec2(dot(-origin, rect.tangent1) / width,
                             dot(-origin, rect.tangent2) / height);
  t_nearest_edge = clamp(t_nearest_edge, vec2(0), vec2(1));
  
  vec2 weight = abs(vec2(dot(reflection_direction, rect.tangent1),
                         dot(reflection_direction, rect.tangent2)));
                         
  vec2 t = mix(t_lines, t_nearest_edge, 0);
  
  vec3 nearest_point = origin + t.x*axis1 + t.y*axis2;
  
  light_distance = length(nearest_point);
  float radius = mix(rect.half_width, rect.half_height, 0.5f);
  
  vec3 l = nearest_point / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(radius, light_distance, surface);
  return l;
}