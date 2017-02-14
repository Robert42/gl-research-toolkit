#version 450 core
#include <extensions/common.glsl>

#define COMPUTE_SHADER

#include <scene/uniforms.glsl>
#include <voxels/distance-field-utils.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct MergeHeader
{
  layout(rgba16f) writeonly image3D targetTexture;
  vec4 gridLocation;
  uvec3 targetTextureSize;
};

layout(binding=UNIFORM_MERGE_SDF_BLOCK, std140)
uniform MergeHeaderBlock
{
  MergeHeader header;
};

void main()
{
    vec3 gridLocationOffset = scene.candidateGridHeader.fallbackSdfGridLocation.xyz;
    float gridLocationScale = scene.candidateGridHeader.fallbackSdfGridLocation.w;
    
    uvec3 voxel_index = gl_GlobalInvocationID;
    vec3 voxel_position = vec3(0.5) + vec3(voxel_index);

    vec3 world_pos = (voxel_position - gridLocationOffset)/gridLocationScale;
    
    
    Sphere* leaf_bounding_spheres = distance_fields_bounding_spheres();
    VoxelDataBlock* leaf_data_blocks = distance_fields_voxelData();
    uint32_t num_distance_fields = distance_fields_num();
    
    float bestDistance = inf;
    vec3 bestPoint = vec3(0);
    
    for(uint32_t i=0; i<num_distance_fields; ++i)
    {
      VoxelDataBlock* distance_field_data_block = leaf_data_blocks + i;
      
      mat4x3 worldToVoxelSpace;
      ivec3 voxelSize;
      vec3 voxelToUvwSpace;
      float worldToVoxelSpace_Factor;
      sampler3D texture = distance_field_data(distance_field_data_block, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
      
      vec3 p = transform_point(worldToVoxelSpace, world_pos);
      vec3 currentPoint;
      float currentDistance = distancefield_distance_resolution_returning_point(p, voxelToUvwSpace, texture, voxelSize, currentPoint);
      
      // scale from source sdf voxel-space to world-space
      currentDistance /= (length(worldToVoxelSpace[0]) + length(worldToVoxelSpace[1]) + length(worldToVoxelSpace[2])) / 3.f;
      
      // scale from world-space to target sdf voxel-space
      currentDistance *= gridLocationScale;
      
      bestPoint = mix(bestPoint, currentPoint, step(currentDistance, bestDistance));
      bestDistance = min(bestDistance, currentDistance);
    }
    
    imageStore(header.targetTexture, ivec3(voxel_index), vec4(normalize(bestPoint-world_pos), bestDistance));
}
