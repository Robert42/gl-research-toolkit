
vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Disk disk, in SurfaceData surface, in vec3 dominant_reflection_direction)
{
  // TODO
  specularEnergyFactor = 1.f;
  light_distance = length(disk.origin-surface.position);
  return (disk.origin-surface.position) / light_distance;
}


vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Rect rect, in SurfaceData surface, in vec3 dominant_reflection_direction)
{
  float rect_radius = mix(rect.half_width, rect.half_height, 0.5f);

  // TODO improve performance
  rect.origin -= surface.position;
  
  Plane rect_plane = plane_from_rect(rect);
  
  Ray reflection_ray;
  reflection_ray.direction = dominant_reflection_direction;
  reflection_ray.origin = vec3(0);
  
  Ray r = reflection_ray;
  r.origin += surface.position;
  
  
  if(!intersects_unclamped(rect, reflection_ray))
  {
    // 1. project the rect along a ray to a plane perpendicular to the ray direction
    vec3 p[4];
    p[0] = rect.origin + -rect.tangent1*rect.half_width + rect.tangent2*rect.half_height;
    p[1] = rect.origin + -rect.tangent1*rect.half_width - rect.tangent2*rect.half_height;
    p[2] = rect.origin +  rect.tangent1*rect.half_width - rect.tangent2*rect.half_height;
    p[3] = rect.origin +  rect.tangent1*rect.half_width + rect.tangent2*rect.half_height;
    
    // 1.a) the position of the plane is as close as possible to the rect to prevent errors 
    const float distance_to_projection_plane = min(rect.half_width, rect.half_height);
    float image_plane = min4(dot(reflection_ray.direction, p[0]),
                             dot(reflection_ray.direction, p[1]),
                             dot(reflection_ray.direction, p[2]),
                             dot(reflection_ray.direction, p[3]));
    // 1.b) the projection center is still on the ray, but roughly the radius of the light away from the light to reduce artefacts from projecting a too narrow rectangle
    vec3 projection_center = reflection_ray.direction * (image_plane-distance_to_projection_plane);
    reflection_ray.origin = projection_center;
    
    // 1.c) the intersection of the ray with the projection plane
    vec3 image_center = get_point(reflection_ray, distance_to_projection_plane);
    Plane projection_plane = plane_from_normal(reflection_ray.direction, image_center);
    
    // 1.d) finally project the rect onto the plane
    for(int i=0; i<4; ++i)
      p[i] = perspective_projection_unclamped(projection_plane, projection_center, p[i]);
      
    vec4 distances;
    vec3 nearest[4];
    
    // 2. for each edge of the projected rect, find the nearest point on the edge
    for(int i=0; i<4; ++i)
    {
      int j = (i+1) % 4;
      nearest[i] = nearest_point_to_line_segment(p[i], p[j], image_center);
      distances[i] = sq(nearest[i]-image_center);
    }
    
    // 2.a) find the nearest point of all edges
    int best_index = index_of_min_component(distances);
    
    vec3 best_point = nearest[best_index];
  
    // 3. Get the direction from the suface position to the found point on the real rectangle
    reflection_ray.direction = normalize(best_point-projection_center);
  }
  
  vec3 mrp;
  intersection_point_unclamped(rect_plane, reflection_ray, mrp);
  
  light_distance = length(mrp);
  vec3 l = mrp / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(rect_radius, light_distance, surface);
  
  return l;
}