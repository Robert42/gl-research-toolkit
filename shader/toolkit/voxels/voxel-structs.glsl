struct VoxelData
{
  mat4 worldToVoxelSpace;
  ivec3 voxelCount;
  uint64_t gpuTextureHandle;
};