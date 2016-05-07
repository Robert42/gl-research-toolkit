#version 450 core
#include <extensions/common.glsl>

#include <glrt/glsl/math.h>
#include <openvdb/triangle-distance.glsl>




layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct VoxelizeMetaData
{
  int numVertices;
  bool two_sided;
  float offset;
  float factor;
  layout(r32f) image3D targetTexture;
  uint64_t vertices;
};

layout(binding=0, std140)
uniform VoxelizeMetaDataBlock
{
  VoxelizeMetaData metaData;
};

void main()
{
  int num_vertices = metaData.numVertices;
  vec3* vertices = vec3*(metaData.vertices);
  bool two_sided = metaData.two_sided;
  
  float best_positive_d = inf;
  float best_negative_d = -inf;
  float best_d_abs = inf;
  
  ivec3 textureSize = imageSize(metaData.targetTexture);
  
  const ivec3 voxelCoord = voxelIndexFromScalarIndex(int(gl_GlobalInvocationID.x), textureSize);
  const vec3 p = centerPointOfVoxel(voxelCoord);

  for(int i=0; i<num_vertices; i+=3)
  {
    const vec3 v0 = vertices[i];
    const vec3 v1 = vertices[i+1];
    const vec3 v2 = vertices[i+2];

    vec3 uvw;
    vec3 closestPoint = closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);

    float d_abs = distance(closestPoint, p);
    float d = -faceforward(vec3(d_abs,0,0), cross(v1-v0, v2-v0), p-closestPoint).x;
    
    best_d_abs = min(d_abs, best_d_abs);
    best_positive_d = d >= 0 ? min(d, best_positive_d) : best_positive_d;
    best_negative_d = d <= 0 ? max(d, best_negative_d) : best_negative_d;
  }
  
  float best_d;
  
  if(two_sided)
  {
    best_d = best_d_abs;
  }else
  {
    if(abs(best_negative_d) + 1.e-5f < best_positive_d)
      best_d = best_negative_d;
    else
      best_d = best_positive_d;
  }
  
  if(all(lessThan(voxelCoord, textureSize)))
    imageStore(metaData.targetTexture, voxelCoord, vec4(best_d));
}
