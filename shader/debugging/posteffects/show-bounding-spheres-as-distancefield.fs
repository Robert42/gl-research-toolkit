#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-bounding-spheres.glsl>

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
  world_normal = vec3(0);
  
  Sphere* bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* distance_field_data_blocks = distance_fields_voxelData();
  uint32_t num_distance_fields = distance_fields_num();
  
  PRINT_VALUE(bounding_spheres->origin);
  PRINT_VALUE(bounding_spheres->radius);
  
  uint32_t stepCount = 0;
  
  bool hit = raymarch_boundingspheres_as_distancefield(ray, bounding_spheres, distance_field_data_blocks, num_distance_fields, world_pos, world_normal, stepCount);
    
  if(posteffect_param.showNumSteps)
  {
    if(hit)
      world_pos = near_plane_world_pos;
    else
      world_pos = far_plane_world_pos;
    color = heatvision(stepCount);
    return;
  }
  
  if(!hit)
    discard;
}
