#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/global-distance-field.glsl>

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
  world_normal = vec3(0);
  
  GlobalDistanceField distance_field = init_global_distance_field();
  
  uint32_t stepCount = 0;
  
  bool hit = raymarch(distance_field, ray, world_pos, stepCount);
    
  if(posteffect_param.showNumSteps)
  {
    if(hit)
      world_pos = near_plane_world_pos;
    else
      world_pos = far_plane_world_pos;
    color = heatvision(stepCount);
    return;
  }else
  {
    uint32_t dummy;
    world_normal = distancefield_normal(distance_field, world_pos, dummy);
  }
  
  if(!hit)
    discard;
}
