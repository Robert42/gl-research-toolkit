// Source: http://iquilezles.org/www/material/nvscene2008/rwwtt.pdf Page 46
//     See also http://iquilezles.org/www/material/nvscene2008/nvscene2008.htm
float distancefield_gradient_1d(vec3 voxelCoord, in vec3 voxelToUvwSpace, sampler3D texture, vec3 epsilon)
{
  return distancefield_distance(voxelCoord + epsilon, voxelToUvwSpace, texture) - distancefield_distance(voxelCoord - epsilon, voxelToUvwSpace, texture);
}

vec3 distancefield_normal(vec3 voxelCoord, in vec3 voxelToUvwSpace, sampler3D texture, float epsilon=0.5)
{
  return normalize(vec3(distancefield_gradient_1d(voxelCoord, voxelToUvwSpace, texture, vec3(epsilon, 0, 0)),
                        distancefield_gradient_1d(voxelCoord, voxelToUvwSpace, texture, vec3(0, epsilon,  0)),
                        distancefield_gradient_1d(voxelCoord, voxelToUvwSpace, texture, vec3(0, 0, epsilon))));
}

void distancefield_normal_samplepoints(in vec3 coord, out vec3 x1, out vec3 x2, out vec3 y1, out vec3 y2, out vec3 z1, out vec3 z2, float epsilon=0.001f)
{
  x1 = coord + vec3(epsilon, 0, 0);
  x2 = coord - vec3(epsilon, 0, 0);
  y1 = coord + vec3(0, epsilon, 0);
  y2 = coord - vec3(0, epsilon, 0);
  z1 = coord + vec3(0, 0, epsilon);
  z2 = coord - vec3(0, 0, epsilon);
}

vec3 distancefield_normal(float dx1, float dx2, float dy1, float dy2, float dz1, float dz2)
{
  return normalize(vec3(dx1-dx2, dy1-dy2, dz1-dz2));
}
