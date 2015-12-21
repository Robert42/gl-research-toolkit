
vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Disk disk, in SurfaceData surface, in vec3 dominant_reflection_direction)
{
  // TODO
  specularEnergyFactor = 1.f;
  light_distance = length(disk.origin-surface.position);
  return (disk.origin-surface.position) / light_distance;
}

void print_proj(Plane plane, vec3 p, vec3 offset)
{
  Ray ray;
  ray.origin = vec3(0);
  ray.direction = normalize(p);

  vec3 ip;
  intersection_point(plane, ray, ip);
  PRINT_VALUE(ip+offset, true);

  ray.origin = offset;
  PRINT_VALUE(ray, true);
}

#define PRINT_PROJ(p) print_proj(rect_plane, p, surface.position)

vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Rect rect, in SurfaceData surface, in vec3 dominant_reflection_direction)
{
  // TODO improve performance
  rect.origin -= surface.position;
  
  Plane rect_plane = plane_from_rect(rect);
  
  Ray reflection_ray;
  reflection_ray.direction = dominant_reflection_direction;
  reflection_ray.origin = vec3(0);
  
  if(!intersects_unclamped(rect, reflection_ray))
  {
    const float image_plane = 1.f;
    
    vec3 image_center = get_point(reflection_ray, image_plane);
    Plane projection_plane = plane_from_normal(reflection_ray.direction, image_plane);
    
    vec3 p[4];
    p[0] = rect.origin + -rect.tangent1*rect.half_width + rect.tangent2*rect.half_height;
    p[1] = rect.origin + -rect.tangent1*rect.half_width - rect.tangent2*rect.half_height;
    p[2] = rect.origin +  rect.tangent1*rect.half_width - rect.tangent2*rect.half_height;
    p[3] = rect.origin +  rect.tangent1*rect.half_width + rect.tangent2*rect.half_height;
    
    for(int i=0; i<4; ++i)
      p[i] = perspective_projection_unclamped(projection_plane, vec3(0), p[i]);
      
    vec4 distances;
    
    PRINT_PROJ(image_center);
    
    for(int i=0; i<4; ++i)
    {
      PRINT_PROJ(p[i]);
      int j = (i+1) % 4;
      p[i] = closestPointToLine_twoPoints(p[i], p[j], dominant_reflection_direction);
      distances[i] = sq(p[i]-image_center);
        PRINT_PROJ(p[i]);
    }
    
    vec3 best_point = p[index_of_min_component(distances)];
  
    reflection_ray.direction = normalize(best_point);
  }
  
  light_distance = intersection_distance(rect_plane, reflection_ray);
  
  float radius = mix(rect.half_width, rect.half_height, 0.5f);
  
  vec3 l = reflection_ray.direction;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(radius, light_distance, surface);
  
  
  reflection_ray.origin = surface.position;
  
  return l;
}