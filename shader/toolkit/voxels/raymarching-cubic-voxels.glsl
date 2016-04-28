const float voxelgrid_epsilon = 1.e-5f;

Ray ray_world_to_voxelspace(in Ray ray, in VoxelData_AABB aabb)
{
    return transform_ray(aabb.worldToVoxelSpace, ray);
}

bool is_within_voxel_grid(in VoxelData_AABB aabb, in vec3 pos_voxelspace)
{
  bvec3 notTooSmall = greaterThanEqual(pos_voxelspace, vec3(0));
  bvec3 notTooLarge = lessThan(pos_voxelspace, vec3(aabb.voxelCount));
  
  return all(notTooSmall && notTooLarge);
}

bool is_valid_voxel_index(in VoxelData_AABB aabb, in ivec3 voxel_index)
{
  bvec3 notTooSmall = greaterThanEqual(voxel_index, ivec3(0));
  bvec3 notTooLarge = lessThan(voxel_index, aabb.voxelCount);
  
  return all(notTooSmall && notTooLarge);
}

bool enter_cubic_voxel_grid_voxelspace(inout Ray ray_voxelspace, in VoxelData_AABB aabb, out int dimension)
{
  const float epsilon = voxelgrid_epsilon;
  
  float intersection_distance;
  bool valid_intersection = intersects_aabb(ray_voxelspace, vec3(0), vec3(aabb.voxelCount), intersection_distance, dimension);
  
  vec3 p_voxelspace = get_point(ray_voxelspace, intersection_distance);
  ray_voxelspace.origin = clamp(p_voxelspace, vec3(0), vec3(aabb.voxelCount)-epsilon);
  
  return valid_intersection;
}

void next_cubic_grid_cell_voxelspace(inout Ray ray_voxelspace, in VoxelData_AABB aabb, inout ivec3 voxelCoord, out int dimension)
{
  const float epsilon = voxelgrid_epsilon;
  
  vec3 distances = intersection_distance_to_grid(ray_voxelspace, vec3(0), vec3(aabb.voxelCount));
  
  int i = index_of_min_component_masked(distances, not_(equal(vec3(0), ray_voxelspace.direction)));
  
  voxelCoord[i] += sign(ray_voxelspace.direction);
  
  ray_voxelspace.origin = get_point(ray_voxelspace, distances[i]);
  ray_voxelspace.origin = clamp(ray_voxelspace.origin, ray_voxelspace.origin, vec3(voxelCoord+1-epsilon));
}

bool raymarch_voxelgrid(in Ray ray_worldspace, in VoxelData_AABB aabb, in sampler3D voxelTexture, int treshold, out vec3 intersection_point, out int dimension)
{
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, aabb);

  if(!enter_cubic_voxel_grid_voxelspace(ray_voxelspace, aabb, dimension))
    return false;
  
  ivec3 voxelCoord = ivec3(floor(ray_voxelspace.origin));
  
  while(is_valid_voxel_index(aabb, voxelCoord))
  {
     float voxel_value = texelFetch(voxelTexture, voxelCoord, 0).r;
     
     if(voxel_value <= treshold)
     {
       intersection_point = ray_voxelspace.origin;
       return true;
     }
     
     next_cubic_grid_cell_voxelspace(ray_voxelspace, aabb, voxelCoord, dimension);
  }
  
  return false;
}

vec3 cubic_voxel_surface_normal(in Ray ray, int hit_dimension)
{
  vec3 normal = vec3(0);
  
  normal[hit_dimension] = sign(-ray.direction[hit_dimension]);
  
  PRINT_VALUE(hit_dimension);
  
  return normal;
}