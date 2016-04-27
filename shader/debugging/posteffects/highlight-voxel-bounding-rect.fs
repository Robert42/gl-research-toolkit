#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-cubic-voxels.glsl>

vec3 rayMarch(in Ray ray, out vec4 color)
{
  color = vec4(0.5, 0.25, 0, 1);
  
  uint32_t num_distance_fields;
  VoxelData_AABB* distance_field_data_aabb;
  get_distance_field_data(num_distance_fields, distance_field_data_aabb);
  
  float nearest_distance = inf;
  vec3 final_world_pos;
  
  for(int i=0; i<num_distance_fields; ++i)
  {
    VoxelData_AABB voxelData = distance_field_data_aabb[i];
    Ray r = transform_ray(voxelData.worldToVoxelSpace, ray);
    
    float intersection_distance;
    int intersection_dimension;
    if(intersects_aabb(r, vec3(0), vec3(voxelData.voxelCount), intersection_distance, intersection_dimension))
    {
      vec3 p_voxelspace = get_point(r, intersection_distance);
      vec3 p_worldspace = transform_point(inverse(voxelData.worldToVoxelSpace), p_voxelspace);
      
      float current_distance = sq_distance(p_worldspace, ray.origin);
      if(nearest_distance > current_distance)
      {
        nearest_distance = current_distance;
        color.rgb = encode_signed_normalized_vector_as_color(cubic_voxel_surface_normal(r, intersection_dimension));
        final_world_pos = p_worldspace;
      }
    }
  }
  
  if(isinf(nearest_distance))
    discard;
  
  return final_world_pos;
}
