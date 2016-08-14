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

  typedef scene::resources::BoundingSphere BoundingSphere;

  typedef scene::Scene::Data::VoxelGrids VoxelGrids;
  typedef scene::Scene::Data::VoxelBVHs VoxelBVH;

  const VoxelGrids& leaves;
  const VoxelBVH& nodes;

  BVH(const VoxelGrids& voxelGridData, VoxelBVH& voxelBvhData);
  void updateTreeCPU(BoundingSphere* bvhInnerBoundingSpheres, InnerNode* bvhInnerNodes);

private:
  BoundingSphere* bvhInnerBoundingSpheres = nullptr;
  BVH::InnerNode* bvhInnerNodes = nullptr;
  quint16 numInnerNodes = 0;
  const quint16 innerNodesCapacity;

  quint16 addInnerNode();

  quint16 generateHierarchy(quint16 begin, quint16 end);
  quint16 findSplit(quint16 begin, quint16 end);

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
  typedef scene::Scene::Data::VoxelGrids VoxelGrid;

  glrt::scene::Scene& scene;
  VoxelGrid& voxelGridData;
  VoxelBVH& voxelBvh;

  ManagedGLBuffer<scene::resources::VoxelUniformDataBlock> distanceFieldVoxelData;
  ManagedGLBuffer<BoundingSphere> distanceFieldboundingSpheres;


  ManagedGLBuffer<BoundingSphere> bvhInnerBoundingSpheres;
  ManagedGLBuffer<BVH::InnerNode> bvhInnerNodes;

  VoxelHeader _voxelHeader;

  void updateVoxelGrid();
  void updateBvhTree();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
