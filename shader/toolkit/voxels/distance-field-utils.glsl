#ifndef VOXELS_DISTANCEFIELD_UTILS_GLSL
#define VOXELS_DISTANCEFIELD_UTILS_GLSL

#include "voxel-structs.glsl"

float distancefield_distance_clamp_range(vec3 voxelCoord, in vec3 voxelToUvwSpace, sampler3D voxelTexture, in vec3 clampRange)
{
  vec3 clamped_voxelCoord = clamp(voxelCoord, vec3(0.5), clampRange);

  float voxel_value = texture(voxelTexture, voxelToUvwSpace * clamped_voxelCoord).w;

#ifdef POSTEFFECT_VISUALIZATION
  voxel_value += posteffect_param.distancefield_offset;
#endif

#if AO_SPHERETRACE_CLAMPING_CORRECTION
  voxel_value += distance(clamped_voxelCoord, voxelCoord);
#endif

  return voxel_value;
}

float distancefield_distance_resolution(vec3 voxelCoord, in vec3 voxelToUvwSpace, sampler3D voxelTexture, in ivec3 voxelResolution)
{
  vec3 clampRange = vec3(voxelResolution)-vec3(0.5);

  return distancefield_distance_clamp_range(voxelCoord, voxelToUvwSpace, voxelTexture, clampRange);
}

float distancefield_distance(vec3 voxelCoord, in vec3 voxelToUvwSpace, sampler3D voxelTexture)
{
  ivec3 voxelResolution = ivec3(textureSize(voxelTexture, 0));

  return distancefield_distance_resolution(voxelCoord, voxelToUvwSpace, voxelTexture, voxelResolution);
}

#include <distance-fields/debugging.glsl>

Ray ray_world_to_voxelspace(in Ray ray, in mat4x3 worldToVoxelSpace)
{
    return transform_ray(worldToVoxelSpace, ray);
}

vec3 point_voxel_to_worldspace_slow(in vec3 p, in mat4x3 worldToVoxelSpace)
{
    return transform_point(inverseOf4x3As4x4(worldToVoxelSpace), p);
}

vec3 direction_voxel_to_worldspace_slow(in vec3 d, in mat4x3 worldToVoxelSpace)
{
    return normalize(transform_direction(inverseOf4x3As4x4(worldToVoxelSpace), d));
}

bool is_within_voxel_grid(in ivec3 voxelCount, in vec3 pos_voxelspace)
{
  bvec3 notTooSmall = greaterThanEqual(pos_voxelspace, vec3(0));
  bvec3 notTooLarge = lessThan(pos_voxelspace, vec3(voxelCount));
  
  return all(notTooSmall && notTooLarge);
}

#endif // VOXELS_DISTANCEFIELD_UTILS_GLSL
