float distancefield_distance(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D voxelTexture)
{
  float voxel_value = texture(voxelTexture, spaceFactor.voxelToUvwSpace * voxelCoord).r;
  
  voxel_value += posteffect_param.distancefield_offset;;
  
  return voxel_value;
}

#include <distance-fields/debugging.glsl>
