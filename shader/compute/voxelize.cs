#version 450 core
#include <extensions/common.glsl>

#include <glrt/glsl/math.h>
#include <opencv/triangle-distance.glsl>

struct VoxelizeMetaData
{
  int numVertices;
  bool two_sided;
  float offset;
  float factor;
  uint64_t vertices;
  image3D targetTexture;
};

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform VoxelizeMetaDataBlock
{
  VoxelizeMetaData metaData;
};

void main()
{
  int num_vertices = metaData.numVertices;
  vec3* vertices = vec3*(metaData.vertices);
  bool two_sided = metaData.two_sided;
  
  float best_d = inf;
  float best_d_abs = inf;
  
  const ivec3 voxelCoord = gl_GlobalInvocationID;
  const vec3 p = centerPointOfVoxel(voxelCoord);
    
  for(int i=0; i<num_vertices; i+=3)
  {
    const vec3 v0 = vertices[i];
    const vec3 v1 = vertices[i+1];
    const vec3 v2 = vertices[i+2];

    vec3 uvw;
    vec3 closestPoint = openvdb::math::closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);

    float d_abs = distance(closestPoint, p);
    float d = -faceforward(vec3(d_abs,0,0), glm::cross(v1-v0, v2-v0), p-closestPoint).x;

    if(best_d_abs > d_abs)
    {
      best_d = d;
      best_d_abs = d_abs;
    }
  }
  
  if(two_sided)
    best_d = best_d_abs;
    
  ivec3 textureSize = imageSize(metaData.targetTexture);
  
  if(all(lessThan(voxelCoord, textureSize)))
    imageStore(metaData.targetTexture, voxelCoord, vec4(best_d));
}
