#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-cubic-voxels.glsl>

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  VoxelDataBlock* distance_field_data_blocks = distance_fields_voxelData();
  uint32_t num_distance_fields = distance_fields_num();
  
  float nearest_distance = inf;
  
  for(int i=0; i<num_distance_fields; ++i)
  {
    mat4x3 worldToVoxelSpace;
    ivec3 voxelSize;
    vec3 voxelToUvwSpace;
    float worldToVoxelSpace_Factor;
    
    sampler3D texture = distance_field_data(distance_field_data_blocks, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
    
    float voxelToWorldSpace_Factor = 1.f/worldToVoxelSpace_Factor;
    
    Ray r = ray_world_to_voxelspace(ray, worldToVoxelSpace);
    
    float intersection_distance;
    int intersection_dimension;
    if(intersects_aabb(r, vec3(0), vec3(voxelSize), intersection_distance, intersection_dimension))
    {
      vec3 p_voxelspace = get_point(r, intersection_distance);
      vec3 p_worldspace = transform_point(inverseOf4x3As4x4(worldToVoxelSpace), p_voxelspace);
      
      float current_distance = sq_distance(p_worldspace, ray.origin);
      if(nearest_distance > current_distance)
      {
        nearest_distance = current_distance;
        world_pos = p_worldspace;
        world_normal = cubic_voxel_surface_normal(r, intersection_dimension);
      }
    }
    
    ++distance_field_data_blocks;
  }
  
  if(isinf(nearest_distance))
    discard;
}
