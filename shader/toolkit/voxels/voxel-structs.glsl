#ifndef VOXELSTRUCTS_GLSL
#define VOXELSTRUCTS_GLSL

#include <alignment.glsl>

struct VoxelDataBlock
{
    vec3 globalWorldToVoxelMatrix_col0;
    int voxelCount_x;
    vec3 globalWorldToVoxelMatrix_col1;
    int voxelCount_y;
    vec3 globalWorldToVoxelMatrix_col2;
    int voxelCount_z;
    vec3 globalWorldToVoxelMatrix_col3;
    float distance_field_worldToVoxelSpaceFactor;

    sampler3D texture;
    padding1(uint64_t, _padding1);
};

mat4x3 distance_field_worldToVoxelSpace(VoxelDataBlock* block)
{
  return mat4x3(block->globalWorldToVoxelMatrix_col0,
                block->globalWorldToVoxelMatrix_col1,
                block->globalWorldToVoxelMatrix_col2,
                block->globalWorldToVoxelMatrix_col3);
}

ivec3 distance_field_voxelCount(VoxelDataBlock* block)
{
  return ivec3(block->voxelCount_x,
               block->voxelCount_y,
               block->voxelCount_z);
}

vec3 distance_field_voxelToUvwSpace(in ivec3 voxelCount)
{
  return 1.f / vec3(voxelCount);
}

vec3 distance_field_voxelToUvwSpace(VoxelDataBlock* block)
{
  return distance_field_voxelToUvwSpace(distance_field_voxelCount(block));
}

float distance_field_worldToVoxelSpaceFactor(VoxelDataBlock* block)
{
  return block->distance_field_worldToVoxelSpaceFactor;
}

sampler3D distance_field_texture(VoxelDataBlock* block)
{
  return block->texture;
}

sampler3D distance_field_data(VoxelDataBlock* block, out mat4x3 worldToVoxelSpace, out ivec3 voxelCount, out vec3 voxelToUvwSpace, out float worldToVoxelSpaceFactor)
{
  worldToVoxelSpace = distance_field_worldToVoxelSpace(block);
  voxelCount = distance_field_voxelCount(block);
  voxelToUvwSpace = distance_field_voxelToUvwSpace(voxelCount);
  worldToVoxelSpaceFactor = distance_field_worldToVoxelSpaceFactor(block);
  
  return distance_field_texture(block);
}

#endif //  VOXELSTRUCTS_GLSL