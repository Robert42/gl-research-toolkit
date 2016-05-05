#version 450 core
#include "posteffect.fs.glsl"

#include <voxels/raymarching-distance-field.glsl>

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal)
{
  world_pos = vec3(0);
    world_normal = vec3(0);
    
  float treshold = 0.5f;
  
  uint32_t num_distance_fields = distance_fields_num();
  mat4* worldToVoxelSpaces = distance_fields_worldToVoxelSpace();
  ivec3* voxelCounts = distance_fields_voxelCount();
  WorldVoxelUvwSpaceFactor* spaceFactors = distance_fields_spaceFactor();
  sampler3D* distance_field_textures = distance_fields_texture();
  
  uint32_t stepCount = 0;
  
  bool hit = raymarch_distancefields(ray, worldToVoxelSpaces, spaceFactors, voxelCounts, distance_field_textures, num_distance_fields, treshold, world_pos, world_normal, stepCount);
    
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