#ifndef GLRT_RENDERER_VOXELBUFFER_H
#define GLRT_RENDERER_VOXELBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/simple-shader-storage-buffer.h>


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
    GLuint64 distanceFieldBoundingSpheres;
    GLuint64 distanceFieldDataStorageBuffer;
    //padding<GLuint64, 1> _padding1;
    quint32 numDistanceFields;
    padding<quint32, 3> _padding2;
  };

  VoxelBuffer(scene::Scene& scene);
  ~VoxelBuffer();

  const VoxelHeader& updateVoxelHeader();

  quint32 numVisibleVoxelGrids() const;


private:
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, scene::VoxelDataComponent::VoxelDataBlock, &scene::VoxelDataComponent::voxelDataBlock>> distanceFieldDataStorageBuffer;
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, BoundingSphere, &scene::VoxelDataComponent::boundingSphere>> distanceFieldBoundingSphereStorageBuffer;

  QVector<quint32> zIndices;
  ManagedGLBuffer<BoundingSphere> bvhInnerBoundingSpheres;
  ManagedGLBuffer<BVH::InnerNode> bvhInnerNodes;

  VoxelHeader _voxelHeader;

  quint32 _numVisibleVoxelGrids = 0;

  void updateBvhTree();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
