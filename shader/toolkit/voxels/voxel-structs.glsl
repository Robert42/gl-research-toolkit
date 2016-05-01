struct VoxelBoundingBox
{
  mat4 worldToVoxelSpace;
  ivec3 voxelCount;
};

struct WorldVoxelUvwSpaceFactor
{
  vec3 voxelToUvwSpace;
  float voxelToWorldSpace;
};
