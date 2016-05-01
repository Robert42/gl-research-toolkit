#include "raymarching-cubic-voxels.glsl"
#include "distance-field-utils.glsl"

bool raymarch_distancefield(in Ray ray_worldspace, in mat4* worldToVoxelSpaceMatrices, in WorldVoxelUvwSpaceFactor* spaceFactors, in ivec3* voxelSizes, sampler3D* voxelTextures, uint32_t index, float treshold, out vec3 intersection_point, out vec3 intersection_normal)
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
  
  while(t < intersection_distance_front)
  {
    vec3 p = get_point(ray_voxelspace, t);
    
    float d = distancefield_distance(p, spaceFactor, texture);
    
    if(d <= voxelgrid_epsilon)
    {
      intersection_point = p;
      intersection_normal = distancefield_normal(p, spaceFactor, texture, voxelgrid_epsilon);
      return true;
    }
    
    t += d;
  }
  
  return false;
}

bool raymarch_distancefields(in Ray ray_worldspace, in mat4* worldToVoxelSpaceMatrices, in WorldVoxelUvwSpaceFactor* spaceFactors, in ivec3* voxelSizes, sampler3D* voxelTextures, uint32_t num_voxels, float treshold, out vec3 intersection_point, out vec3 intersection_normal)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    vec3 intersection_point_tmp;
    vec3 intersection_normal_tmp;
    
    bool got_hit = raymarch_distancefield(ray_worldspace, worldToVoxelSpaceMatrices, spaceFactors, voxelSizes, voxelTextures, i, treshold, intersection_point_tmp, intersection_normal_tmp);

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