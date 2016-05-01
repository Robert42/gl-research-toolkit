// Source: http://iquilezles.org/www/material/nvscene2008/rwwtt.pdf Page 46
//     See also http://iquilezles.org/www/material/nvscene2008/nvscene2008.htm
float distancefield_gradient_1d(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D texture, vec3 epsilon)
{
  return distancefield_distance(voxelCoord + epsilon, spaceFactor, texture) - distancefield_distance(voxelCoord - epsilon, spaceFactor, texture);
}

vec3 distancefield_normal(vec3 voxelCoord, in WorldVoxelUvwSpaceFactor spaceFactor, sampler3D texture, float epsilon=0.5)
{
  return normalize(vec3(distancefield_gradient_1d(voxelCoord, spaceFactor, texture, vec3(epsilon, 0, 0)),
                        distancefield_gradient_1d(voxelCoord, spaceFactor, texture, vec3(0, epsilon,  0)),
                        distancefield_gradient_1d(voxelCoord, spaceFactor, texture, vec3(0, 0, epsilon))));
}
