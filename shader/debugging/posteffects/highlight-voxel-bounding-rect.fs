#version 450 core
#include "posteffect.fs.glsl"

vec3 rayMarch(in Ray ray, out vec4 color)
{
  color = vec4(0.5, 0.25, 0, 1);
  
  uint32_t num_distance_fields;
  VoxelData_AABB* distance_field_data_aabb;
  get_distance_field_data(num_distance_fields, distance_field_data_aabb);
  
  for(int i=0; i<num_distance_fields; ++i)
  {
    VoxelData_AABB voxelData = distance_field_data_aabb[i];
    Ray r = transform_ray(voxelData.worldToVoxelSpace, ray);
    
    float intersection_distance;
    if(intersects_aabb(r, vec3(0), vec3(voxelData.voxelCount), intersection_distance))
    {
      vec3 p_voxelspace = get_point(r, intersection_distance);
      vec3 p_worldspace = transform_point(inverse(voxelData.worldToVoxelSpace), p_voxelspace);
      return p_worldspace;
    }
  }
  
  discard;
}
