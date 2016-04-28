const float voxelgrid_epsilon = 1.e-5f;

vec3 cubic_voxel_surface_normal(in Ray ray, int hit_dimension)
{
  vec3 normal = vec3(0);
  
  normal[hit_dimension] = sign(-ray.direction[hit_dimension]);
  
  PRINT_VALUE(hit_dimension);
  
  return normal;
}

Ray ray_world_to_voxelspace(in Ray ray, in VoxelData_AABB aabb)
{
    return transform_ray(aabb.worldToVoxelSpace, ray);
}

vec3 point_voxel_to_worldspace(in vec3 p, in VoxelData_AABB aabb)
{
    return transform_point(inverse(aabb.worldToVoxelSpace), p);
}

vec3 direction_voxel_to_worldspace(in vec3 d, in VoxelData_AABB aabb)
{
    return transform_direction(inverse(aabb.worldToVoxelSpace), d);
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

void next_cubic_grid_cell_voxelspace(inout Ray ray_voxelspace, in ivec3 marchingStep, in VoxelData_AABB aabb, inout ivec3 voxelCoord, out int dimension)
{
  const float epsilon = voxelgrid_epsilon;
  
  vec3 distances = intersection_distance_to_grid(ray_voxelspace, vec3(0), vec3(aabb.voxelCount));
    
  int i = index_of_min_component_masked(distances, not_(equal(vec3(0), ray_voxelspace.direction)));
  
  voxelCoord[i] += marchingStep[i];

  ray_voxelspace.origin = get_point(ray_voxelspace, distances[i]);
  ray_voxelspace.origin = clamp(ray_voxelspace.origin, ray_voxelspace.origin, vec3(voxelCoord+1-epsilon));

  dimension = i;
}

bool raymarch_voxelgrid(in Ray ray_worldspace, in VoxelData_AABB* voxelData, sampler3D* voxelTextures, uint32_t index, float treshold, out vec3 intersection_point, out vec3 intersection_normal)
{
  int hit_dimension;
  VoxelData_AABB aabb = voxelData[index];
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, aabb);

  if(!enter_cubic_voxel_grid_voxelspace(ray_voxelspace, aabb, hit_dimension))
    return false;
    
  sampler3D voxelTexture = voxelTextures[index];
  
  ivec3 marchingStep = ivec3(sign(ray_voxelspace.direction));
  ivec3 voxelCoord = ivec3(floor(ray_voxelspace.origin));

  int max_num_loops = 65536;
  while(is_valid_voxel_index(aabb, voxelCoord) && 0<=max_num_loops--)
  {

     float voxel_value = texelFetch(voxelTexture, voxelCoord, 0).r;
     
     if(voxel_value <= treshold)
     {
       intersection_point = ray_voxelspace.origin;
       intersection_normal = cubic_voxel_surface_normal(ray_voxelspace, hit_dimension);
       
       intersection_point = point_voxel_to_worldspace(intersection_point, aabb);
       intersection_normal = direction_voxel_to_worldspace(intersection_normal, aabb);
       return true;
     }
     
     next_cubic_grid_cell_voxelspace(ray_voxelspace, marchingStep, aabb, voxelCoord, hit_dimension);
  }
  
  return false;
}

bool raymarch_voxelgrids(in Ray ray_worldspace, VoxelData_AABB* voxelData, sampler3D* voxelTextures, uint32_t num_voxels, float treshold, out vec3 intersection_point, out vec3 intersection_normal)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    vec3 intersection_point_tmp;
    vec3 intersection_normal_tmp;
    
    bool got_hit = raymarch_voxelgrid(ray_worldspace, voxelData, voxelTextures, i, treshold, intersection_point_tmp, intersection_normal_tmp);

    float current_distance = sq_distance(intersection_point_tmp, ray_worldspace.origin);
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        intersection_point = intersection_point_tmp;
        intersection_normal = intersection_normal_tmp;
    }
  }
  
  return !isinf(nearest_distance);
}