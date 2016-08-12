#ifndef GLRT_RENDERER_VOXELBUFFER_H
#define GLRT_RENDERER_VOXELBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/toolkit/managed-gl-buffer.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/scene/scene-data.h>


namespace glrt {
namespace renderer {

struct BVH
{
  struct InnerNode
  {
    int leftChild;
    int rightChild;
  };

  const scene::VoxelDataComponent* const * leaves;
  BoundingSphere* const bvhInnerBoundingSpheres;
  InnerNode* const bvhInnerNodes;
  const quint32* const zIndices;
  const int numLeaves;
  const int innerNodesCapacity;
  int numInnerNodes;

  BVH(const scene::VoxelDataComponent* const * leaves, BoundingSphere* bvhInnerBoundingSpheres, InnerNode* bvhInnerNodes, const quint32* zIndices, int numLeaves, int innerNodesCapacity);
  void updateTreeCPU();
  int addInnerNode();

  int generateHierarchy(int begin, int end);
  int findSplit(int begin, int end);

private:
  quint32 zIndexDistance(int a, int b);
};

class VoxelBuffer
{
public:
  struct VoxelHeader
  {
    GLuint64 distanceFieldBvhInnerBoundingSpheres;
    GLuint64 distanceFieldBvhInnerNodes;
    GLuint64 distanceFieldBoundingSpheres;
    GLuint64 distanceFieldDataStorageBuffer;
    //padding<GLuint64, 1> _padding1; // necessary only, if the number of GLuint64%2 != 0
    quint32 numDistanceFields;
    padding<quint32, 3> _padding2;
  };

  VoxelBuffer(scene::Scene& scene);
  ~VoxelBuffer();

  const VoxelHeader& updateVoxelHeader();

private:
  typedef scene::Scene::Data::VoxelBVHs VoxelBVH;
  typedef scene::Scene::Data::VoxelGrids VoxelGrids;

  VoxelGrids& voxelGridData;
  VoxelBVH& voxelBvh;

  ManagedGLBuffer<BoundingSphere> distanceFieldboundingSpheres;
  ManagedGLBuffer<scene::resources::VoxelUniformDataBlock> distanceFieldVoxelData;


  ManagedGLBuffer<BoundingSphere> bvhInnerBoundingSpheres;
  ManagedGLBuffer<BVH::InnerNode> bvhInnerNodes;

  VoxelHeader _voxelHeader;

  void updateBvhTree();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
