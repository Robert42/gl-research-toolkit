#include "raymarching-cubic-voxels.glsl"
#include "distance-field-utils.glsl"

bool raymarch_distancefield(in Ray ray_worldspace, in mat4* worldToVoxelSpaceMatrices, in WorldVoxelUvwSpaceFactor* spaceFactors, in ivec3* voxelSizes, sampler3D* voxelTextures, uint32_t index, float treshold, out float ray_hit_distance_worldspace, out vec3 intersection_normal_worldspace, inout uint32_t stepCount)
{
  mat4 worldToVoxelSpace = worldToVoxelSpaceMatrices[index];
  ivec3 voxelSize = voxelSizes[index];
  WorldVoxelUvwSpaceFactor spaceFactor = spaceFactors[index];
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, worldToVoxelSpace);
  
  float intersection_distance_front;
  float intersection_distance_back;
  
  if(!intersects_aabb_twice(ray_voxelspace, vec3(0), vec3(voxelSize), intersection_distance_front, intersection_distance_back))
    return false;
    
  sampler3D texture = voxelTextures[index];
    
  float t = intersection_distance_front;
  
  int max_num_loops = 65536;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    ++stepCount;
    vec3 p = get_point(ray_voxelspace, t);
    
    float d = distancefield_distance(p, spaceFactor, texture);
    
    if(d <= 1.e-2f)
    {
      ray_hit_distance_worldspace = spaceFactor.voxelToWorldSpace * t;
      intersection_normal_worldspace = transform_direction(inverse(worldToVoxelSpace), distancefield_normal(p, spaceFactor, texture));
      return true;
    }
    
    t += d;
  }
  
  return false;
}

bool raymarch_distancefields(in Ray ray_worldspace, in mat4* worldToVoxelSpaceMatrices, in WorldVoxelUvwSpaceFactor* spaceFactors, in ivec3* voxelSizes, sampler3D* voxelTextures, uint32_t num_voxels, float treshold, out vec3 intersection_point, out vec3 intersection_normal, inout uint32_t stepCount)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    ++stepCount;
    
    float intersection_ray_distance_worldspace;
    vec3 intersection_normal_worldspace;
    
    bool got_hit = raymarch_distancefield(ray_worldspace, worldToVoxelSpaceMatrices, spaceFactors, voxelSizes, voxelTextures, i, treshold, intersection_ray_distance_worldspace, intersection_normal_worldspace, stepCount);

    float current_distance = intersection_ray_distance_worldspace;
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        intersection_normal = intersection_normal_worldspace;
    }
  }
  
  intersection_point = get_point(ray_worldspace, nearest_distance);
  
  return !isinf(nearest_distance);
}