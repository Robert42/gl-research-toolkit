#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-distance-field.glsl>


bool raymarch_fallback_distancefield(in Ray ray_worldspace, out vec3 intersection_pos_worldspace, out vec3 intersection_normal_worldspace, inout uint32_t stepCount);

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
  world_normal = vec3(0);
    
  uint32_t stepCount = 0;
  
  bool hit = raymarch_fallback_distancefield(ray, world_pos, world_normal, stepCount);
    
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


bool raymarch_fallback_distancefield(in Ray ray_worldspace, out vec3 intersection_pos_worldspace, out vec3 intersection_normal_worldspace, inout uint32_t stepCount)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = fallback_distance_field_data(worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  float ray_hit_distance_worldspace = inf;
  bool has_hit = raymarch_distancefield(ray_worldspace, texture, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor, ray_hit_distance_worldspace, intersection_normal_worldspace, stepCount);
  
  intersection_pos_worldspace = get_point(ray_worldspace, ray_hit_distance_worldspace);
  
  //ivec3 _uvw = ivec3(gl_FragCoord.xy/8, 0);
  //vec4 _color = texelFetch(texture, _uvw, 0);
  //PRINT_VALUE(ray_hit_distance_worldspace);
  //PRINT_VALUE(_uvw);
  //PRINT_VALUE(_color);
  
  return has_hit;
}
