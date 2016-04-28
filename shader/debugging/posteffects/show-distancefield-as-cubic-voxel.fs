#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-cubic-voxels.glsl>

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
    world_normal = vec3(0);
    
  float treshold = 0.5f;
  
  uint32_t num_distance_fields;
  VoxelData_AABB* distance_field_data_aabb;
  sampler3D* distance_field_textures;
  get_distance_field_data(num_distance_fields, distance_field_data_aabb, distance_field_textures);
  
  if(!raymarch_voxelgrids(ray, distance_field_data_aabb, distance_field_textures, num_distance_fields, treshold, world_pos, world_normal))
    discard;
}
