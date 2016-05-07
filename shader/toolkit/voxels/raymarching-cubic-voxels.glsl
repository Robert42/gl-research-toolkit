const float voxelgrid_epsilon = 1.e-5f;

vec3 cubic_voxel_surface_normal(in Ray ray, int hit_dimension)
{
  vec3 normal = vec3(0);
  
  normal[hit_dimension] = sign(-ray.direction[hit_dimension]);
  
  return normal;
}

Ray ray_world_to_voxelspace(in Ray ray, in mat4 worldToVoxelSpace)
{
    return transform_ray(worldToVoxelSpace, ray);
}

vec3 point_voxel_to_worldspace(in vec3 p, in mat4 worldToVoxelSpace)
{
    return transform_point(inverse(worldToVoxelSpace), p);
}

vec3 direction_voxel_to_worldspace_slow(in vec3 d, in mat4 worldToVoxelSpace)
{
    return normalize(transform_direction(inverse(worldToVoxelSpace), d));
}

bool is_within_voxel_grid(in ivec3 voxelCount, in vec3 pos_voxelspace)
{
  bvec3 notTooSmall = greaterThanEqual(pos_voxelspace, vec3(0));
  bvec3 notTooLarge = lessThan(pos_voxelspace, vec3(voxelCount));
  
  return all(notTooSmall && notTooLarge);
}

bool is_valid_voxel_index(in ivec3 voxelCount, in ivec3 voxel_index)
{
  bvec3 notTooSmall = greaterThanEqual(voxel_index, ivec3(0));
  bvec3 notTooLarge = lessThan(voxel_index, voxelCount);
  
  return all(notTooSmall && notTooLarge);
}

bool enter_cubic_voxel_grid_voxelspace(inout Ray ray_voxelspace, in ivec3 voxelCount, out int dimension)
{
  const float epsilon = voxelgrid_epsilon;
  
  float intersection_distance;
  bool valid_intersection = intersects_aabb(ray_voxelspace, vec3(0), vec3(voxelCount), intersection_distance, dimension);
  
  vec3 p_voxelspace = get_point(ray_voxelspace, intersection_distance);
  ray_voxelspace.origin = clamp(p_voxelspace, vec3(0), vec3(voxelCount)-epsilon);
  
  return valid_intersection && intersection_distance>=0.f;
}

void next_cubic_grid_cell_voxelspace(inout Ray ray_voxelspace, in ivec3 marchingStep, in ivec3 voxelCount, inout ivec3 voxelCoord, out int dimension)
{
  const float epsilon = voxelgrid_epsilon;
  
  vec3 distances = intersection_distance_to_grid(ray_voxelspace, vec3(0), vec3(voxelCount));
  
  bvec3 valid_direction = not_(equal(vec3(0), ray_voxelspace.direction));
    
  int i = index_of_min_component_masked(distances, valid_direction);
  
  voxelCoord[i] += marchingStep[i];

  ray_voxelspace.origin = get_point(ray_voxelspace, distances[i]);
  ray_voxelspace.origin = clamp(ray_voxelspace.origin, voxelCoord, vec3(voxelCoord+1-epsilon));

  dimension = i;
}

bool raymarch_voxelgrid(in Ray ray_worldspace, in mat4* worldToVoxelSpaces, in ivec3* voxelCounts, sampler3D* voxelTextures, uint32_t index, float treshold, out vec3 intersection_point, out vec3 intersection_normal, inout uint32_t stepCount)
{
  int hit_dimension;
  mat4 worldToVoxelSpace = worldToVoxelSpaces[index];
  ivec3 voxelCount = voxelCounts[index];
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, worldToVoxelSpace);
  
  if(!enter_cubic_voxel_grid_voxelspace(ray_voxelspace, voxelCount, hit_dimension))
    return false;
    
  sampler3D voxelTexture = voxelTextures[index];
  
  ivec3 marchingStep = ivec3(sign(ray_voxelspace.direction));
  ivec3 voxelCoord = ivec3(floor(ray_voxelspace.origin));

  int max_num_loops = 65536;
  while(is_valid_voxel_index(voxelCount, voxelCoord) && 0<=max_num_loops--)
  {
     stepCount++;
    
     float voxel_value = texelFetch(voxelTexture, voxelCoord, 0).r;
     
     voxel_value += posteffect_param.distancefield_offset;
     
     if(voxel_value <= treshold)
     {
       intersection_point = ray_voxelspace.origin;
       intersection_normal = cubic_voxel_surface_normal(ray_voxelspace, hit_dimension);
       
       intersection_point = point_voxel_to_worldspace(intersection_point, worldToVoxelSpace);
       intersection_normal = direction_voxel_to_worldspace_slow(intersection_normal, worldToVoxelSpace);
       return true;
     }
     
     next_cubic_grid_cell_voxelspace(ray_voxelspace, marchingStep, voxelCount, voxelCoord, hit_dimension);
  }
  
  return false;
}

bool raymarch_voxelgrids(in Ray ray_worldspace, in mat4* worldToVoxelSpaces, in ivec3* voxelCounts, sampler3D* voxelTextures, uint32_t num_voxels, float treshold, out vec3 intersection_point, out vec3 intersection_normal, inout uint32_t stepCount)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    stepCount++;
  
    vec3 intersection_point_tmp;
    vec3 intersection_normal_tmp;
    
    bool got_hit = raymarch_voxelgrid(ray_worldspace, worldToVoxelSpaces, voxelCounts, voxelTextures, i, treshold, intersection_point_tmp, intersection_normal_tmp, stepCount);

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