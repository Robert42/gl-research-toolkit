#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-cubic-voxels.glsl>


bool is_unconceiled_negative_distance_singlegrid(in Ray ray_worldspace, in mat4* worldToVoxelSpaces, in ivec3* voxelCounts, sampler3D* voxelTextures, uint32_t index, out vec3 intersection_point, out vec3 intersection_normal)
{
  int hit_dimension;
  mat4 worldToVoxelSpace = worldToVoxelSpaces[index];
  ivec3 voxelCount = voxelCounts[index];
  
  int max_num_loops = 65536;
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, worldToVoxelSpace);
  
  if(!aabb_contains(ray_voxelspace.origin, vec3(0), vec3(voxelCount)))
    max_num_loops = 0;
  
  if(!enter_cubic_voxel_grid_voxelspace(ray_voxelspace, voxelCount, hit_dimension))
    return false;
    
  sampler3D voxelTexture = voxelTextures[index];
  
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
       
       intersection_point = point_voxel_to_worldspace(intersection_point, worldToVoxelSpace);
       intersection_normal = direction_voxel_to_worldspace_slow(intersection_normal, worldToVoxelSpace);
       return true;
     }
     
     prev_voxel_value = voxel_value;
     
     next_cubic_grid_cell_voxelspace(ray_voxelspace, marchingStep, voxelCount, voxelCoord, hit_dimension);
  }
  
  return false;
}


bool is_unconceiled_negative_distance(in Ray ray_worldspace,
                                      in mat4* worldToVoxelSpaces,
                                      in ivec3* voxelCounts,
                                      sampler3D* voxelTextures,
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
    
    bool got_hit = is_unconceiled_negative_distance_singlegrid(ray_worldspace, worldToVoxelSpaces, voxelCounts, voxelTextures, i, intersection_point_tmp, intersection_normal_tmp);

    float current_distance = sq_distance(intersection_point_tmp, ray_worldspace.origin);
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        is_unconceiled_negative_distance = true;
        intersection_point = intersection_point_tmp;
        intersection_normal = intersection_normal_tmp;
    }
  }
  
  return is_unconceiled_negative_distance;
}

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
  world_normal = vec3(0);
  
  uint32_t num_distance_fields = distance_fields_num();
  mat4* worldToVoxelSpaces = distance_fields_worldToVoxelSpace();
  ivec3* voxelCounts = distance_fields_voxelCount();
  sampler3D* distance_field_textures = distance_fields_texture();
  
  if(!is_unconceiled_negative_distance(ray, worldToVoxelSpaces, voxelCounts, distance_field_textures, num_distance_fields, world_pos, world_normal))
    discard;
  
  color.rgb = highlightColor();
  color.a = 1;
}
