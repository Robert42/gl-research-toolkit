float distancefield_distance(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D voxelTexture)
{
  float voxel_value = texture(voxelTexture, spaceFactor.voxelToUvwSpace * voxelCoord).r;
  
  voxel_value += posteffect_param.distancefield_offset;;
  
  return voxel_value;
}

vec3 distancefield_normal(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D texture, float epsilon)
{
  return vec3(0,0,1);
}