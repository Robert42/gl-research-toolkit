#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-cubic-voxels.glsl>

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  uint32_t num_distance_fields = distance_fields_num();
  mat4* worldToVoxelSpaces = distance_fields_worldToVoxelSpace();
  ivec3* voxelCounts = distance_fields_voxelCount();
  
  float nearest_distance = inf;
  
  for(int i=0; i<num_distance_fields; ++i)
  {
    Ray r = ray_world_to_voxelspace(ray, worldToVoxelSpaces[i]);
    
    float intersection_distance;
    int intersection_dimension;
    if(intersects_aabb(r, vec3(0), vec3(voxelCounts[i]), intersection_distance, intersection_dimension))
    {
      vec3 p_voxelspace = get_point(r, intersection_distance);
      vec3 p_worldspace = transform_point(inverse(worldToVoxelSpaces[i]), p_voxelspace);
      
      float current_distance = sq_distance(p_worldspace, ray.origin);
      if(nearest_distance > current_distance)
      {
        nearest_distance = current_distance;
        world_pos = p_worldspace;
        world_normal = cubic_voxel_surface_normal(r, intersection_dimension);
      }
    }
  }
  
  if(isinf(nearest_distance))
    discard;
}
