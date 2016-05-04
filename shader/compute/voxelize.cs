#version 450 core
#include <extensions/common.glsl>

#include <glrt/glsl/math.h>
#include <openvdb/triangle-distance.glsl>



#define SPHERE_ONLY 0

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
  
  float best_d = inf;
  float best_d_abs = inf;
  
  ivec3 textureSize = imageSize(metaData.targetTexture);
  
  const ivec3 voxelCoord = voxelIndexFromScalarIndex(int(gl_GlobalInvocationID.x), textureSize);
  const vec3 p = centerPointOfVoxel(voxelCoord);
    
#if SPHERE_ONLY
  vec3 origin = vec3(textureSize)*.5f;
  float radius = min_component(textureSize)*0.5f - 1.f;
  
  best_d = distance(p, origin) - radius;
  best_d_abs = abs(best_d);
#else
  for(int i=0; i<num_vertices; i+=3)
  {
    const vec3 v0 = vertices[i];
    const vec3 v1 = vertices[i+1];
    const vec3 v2 = vertices[i+2];

    vec3 uvw;
    vec3 closestPoint = closestPointOnTriangleToPoint(v0, v1, v2, p, uvw);

    float d_abs = distance(closestPoint, p);
    float d = -faceforward(vec3(d_abs,0,0), cross(v1-v0, v2-v0), p-closestPoint).x;

    if(best_d_abs > d_abs)
    {
      best_d = d;
      best_d_abs = d_abs;
    }
  }
  
  if(two_sided)
    best_d = best_d_abs;
#endif
  
  if(all(lessThan(voxelCoord, textureSize)))
    imageStore(metaData.targetTexture, voxelCoord, vec4(best_d));
}
