struct VoxelData_AABB
{
  mat4 worldToVoxelSpace;
  ivec3 voxelCount;
  
  uint32_t _padding;
};