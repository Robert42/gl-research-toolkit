#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-cubic-voxels.glsl>


bool is_unconceiled_negative_distance_singlegrid(in Ray ray_worldspace, in VoxelDataBlock* distance_field_data_blocks, out vec3 intersection_point, out vec3 intersection_normal)
{
  int hit_dimension;
  mat4x3 worldToVoxelSpace;
  ivec3 voxelCount;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D voxelTexture = distance_field_data(distance_field_data_blocks, worldToVoxelSpace, voxelCount, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  
  int max_num_loops = 65536;
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, worldToVoxelSpace);
  
  if(!enter_cubic_voxel_grid_voxelspace(ray_voxelspace, voxelCount, hit_dimension))
    return false;
  
  ivec3 voxelCoord = ivec3(floor(ray_voxelspace.origin));
  ivec3 marchingStep = ivec3(sign(ray_voxelspace.direction));
  
  float prev_voxel_value = inf;
  
  while(is_valid_voxel_index(voxelCount, voxelCoord) && 0<=max_num_loops--)
  {
     float voxel_value = texelFetch(voxelTexture, voxelCoord, 0).r;
     
     voxel_value += posteffect_param.distancefield_offset;
     
     if(voxel_value < 0 && distance(voxel_value, prev_voxel_value)>sqrt(2.001f))
     {
       intersection_point = ray_voxelspace.origin;
       intersection_normal = cubic_voxel_surface_normal(ray_voxelspace, hit_dimension);
       
       intersection_point = point_voxel_to_worldspace_slow(intersection_point, worldToVoxelSpace);
       intersection_normal = direction_voxel_to_worldspace_slow(intersection_normal, worldToVoxelSpace);
       return true;
     }
     
     prev_voxel_value = voxel_value;
     
     next_cubic_grid_cell_voxelspace(ray_voxelspace, marchingStep, voxelCount, voxelCoord, hit_dimension);
  }
  
  return false;
}


bool is_unconceiled_negative_distance(in Ray ray_worldspace,
                                      in VoxelDataBlock* distance_field_data_blocks,
                                      uint32_t num_voxels,
                                      out vec3 intersection_point,
                                      out vec3 intersection_normal)
{
  float nearest_distance = inf;
  bool is_unconceiled_negative_distance = false;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    vec3 intersection_point_tmp;
    vec3 intersection_normal_tmp;
    
    bool got_hit = is_unconceiled_negative_distance_singlegrid(ray_worldspace, distance_field_data_blocks, intersection_point_tmp, intersection_normal_tmp);

    float current_distance = sq_distance(intersection_point_tmp, ray_worldspace.origin);
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        is_unconceiled_negative_distance = true;
        intersection_point = intersection_point_tmp;
        intersection_normal = intersection_normal_tmp;
    }
    
    ++distance_field_data_blocks;
  }
  
  return is_unconceiled_negative_distance;
}

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
  world_normal = vec3(0);
  
  VoxelDataBlock* distance_field_data_blocks = distance_fields_voxelData();
  uint32_t num_distance_fields = distance_fields_num();
  
  if(!is_unconceiled_negative_distance(ray, distance_field_data_blocks, num_distance_fields, world_pos, world_normal))
    discard;
  
  color.rgb = highlightColor();
  color.a = 1;
}
