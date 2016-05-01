float distancefield_distance(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D voxelTexture)
{
  return texture(voxelTexture, spaceFactor.voxelToUvwSpace * voxelCoord).r;
}

vec3 distancefield_normal(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D texture, float epsilon)
{
  return vec3(0,0,1);
}