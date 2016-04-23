#version 450 core
#include "posteffect.fs.glsl"

vec3 rayMarch(in Ray ray, out vec4 color)
{
  color = vec4(0.5, 0.25, 0, 1);
  
  uint32_t num_distance_fields;
  VoxelData* distance_field_data;
  get_distance_field_data(num_distance_fields, distance_field_data);
  
  for(int i=0; i<num_distance_fields; ++i)
  {
    VoxelData voxelData = distance_field_data[i];
    Ray r = transform_ray(voxelData.worldToVoxelSpace, ray);
    
    float intersection_distance;
    if(intersects_aabb(r, vec3(0), vec3(voxelData.voxelCount), intersection_distance))
      return get_point(ray, intersection_distance);
  }
  
  PRINT_VALUE(num_distance_fields);
  
  discard;
}