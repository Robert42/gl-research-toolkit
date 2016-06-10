#include "raymarching-cubic-voxels.glsl"
#include "distance-field-utils.glsl"

bool raymarch_distancefield(in Ray ray_worldspace, in VoxelDataBlock* distance_field_data_block, out float ray_hit_distance_worldspace, out vec3 intersection_normal_worldspace, inout uint32_t stepCount)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = distance_field_data(distance_field_data_block, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  float voxelToWorldSpace = 1.f / worldToVoxelSpace_Factor;
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, worldToVoxelSpace);
  
  float intersection_distance_front;
  float intersection_distance_back;
  
  if(!intersects_aabb_twice(ray_voxelspace, vec3(0), vec3(voxelSize), intersection_distance_front, intersection_distance_back))
    return false;
  
  float t = intersection_distance_front;
  
  int max_num_loops = 65536;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    ++stepCount;
    vec3 p = get_point(ray_voxelspace, t);
    
    float d = distancefield_distance(p, voxelToUvwSpace, texture);
    
    if(d <= 1.e-2f)
    {
      ray_hit_distance_worldspace = voxelToWorldSpace * t / worldToVoxelSpace_Factor;
      intersection_normal_worldspace = transform_direction(inverseOf4x3As4x4(worldToVoxelSpace), distancefield_normal(p, voxelToUvwSpace, texture));
      return true;
    }
    
    t += d;
  }
  
  return false;
}

bool raymarch_distancefields(in Ray ray_worldspace, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_voxels, out vec3 intersection_point, out vec3 intersection_normal, inout uint32_t stepCount)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    ++stepCount;
    
    float intersection_ray_distance_worldspace;
    vec3 intersection_normal_worldspace;
    
    bool got_hit = raymarch_distancefield(ray_worldspace, distance_field_data_blocks, intersection_ray_distance_worldspace, intersection_normal_worldspace, stepCount);

    float current_distance = intersection_ray_distance_worldspace;
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        intersection_normal = intersection_normal_worldspace;
    }
    
    ++distance_field_data_blocks;
  }
  
  intersection_point = get_point(ray_worldspace, nearest_distance);
  
  return !isinf(nearest_distance);
}