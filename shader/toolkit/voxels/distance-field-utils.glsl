#ifndef VOXELS_DISTANCEFIELD_UTILS_GLSL
#define VOXELS_DISTANCEFIELD_UTILS_GLSL

#include "voxel-structs.glsl"

float distancefield_distance(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D voxelTexture)
{
  float voxel_value = texture(voxelTexture, spaceFactor.voxelToUvwSpace * voxelCoord).r;
  
#ifdef POSTEFFECT_VISUALIZATION
  voxel_value += posteffect_param.distancefield_offset;
#endif 
  
  return voxel_value;
}

#include <distance-fields/debugging.glsl>

Ray ray_world_to_voxelspace(in Ray ray, in mat4 worldToVoxelSpace)
{
    return transform_ray(worldToVoxelSpace, ray);
}

vec3 point_voxel_to_worldspace(in vec3 p, in mat4 worldToVoxelSpace)
{
    return transform_point(inverse(worldToVoxelSpace), p);
}

vec3 direction_voxel_to_worldspace_slow(in vec3 d, in mat4 worldToVoxelSpace)
{
    return normalize(transform_direction(inverse(worldToVoxelSpace), d));
}

bool is_within_voxel_grid(in ivec3 voxelCount, in vec3 pos_voxelspace)
{
  bvec3 notTooSmall = greaterThanEqual(pos_voxelspace, vec3(0));
  bvec3 notTooLarge = lessThan(pos_voxelspace, vec3(voxelCount));
  
  return all(notTooSmall && notTooLarge);
}

#endif // VOXELS_DISTANCEFIELD_UTILS_GLSL