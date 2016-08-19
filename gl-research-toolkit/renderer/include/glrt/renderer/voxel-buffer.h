#ifndef GLRT_RENDERER_VOXELBUFFER_H
#define GLRT_RENDERER_VOXELBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/toolkit/managed-gl-buffer.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/scene/scene-data.h>


namespace glrt {
namespace glsl {

struct Cone;

} // namespace glsl
namespace renderer {

struct BVH
{
  typedef scene::resources::BoundingSphere BoundingSphere;
  typedef scene::Scene::Data::VoxelGrids VoxelGrids;

  struct InnerNode
  {
    quint16 left_child;
    quint16 right_child;
  };

  struct SubTree
  {
    BoundingSphere bounding_sphere;
    quint16 index;
  };

  const BoundingSphere* const leaves_bounding_spheres;
  const quint32* const leaves_z_indices;
  const quint16 num_leaves;

  BVH(const VoxelGrids& voxelGridData);
  BVH(const BoundingSphere* leaves_bounding_spheres, const quint32* leaves_z_indices, quint16 num_leaves);
  void updateTreeCPU(BoundingSphere* bvhInnerBoundingSpheres, InnerNode* bvhInnerNodes);

  quint16 findSplit(quint16 begin, quint16 end);
  quint32 zIndexDistance(quint16 a, quint16 b);

private:
  BoundingSphere* bvhInnerBoundingSpheres = nullptr;
  BVH::InnerNode* bvhInnerNodes = nullptr;
  quint16 num_inner_nodes = 0;
  const quint16 capacity_inner_nodes;

  quint16 addInnerNode();

  quint16 calcDepth(quint16 root, InnerNode* bvhInnerNodes) const;

  SubTree generateHierarchy(quint16 begin, quint16 end);
  void verifyHierarchy() const;
  void verifyTreeDepth() const;
  void verifyBoundingSpheres(uint16_t root_node=0) const;

  void testOcclusion() const;
  QSet<quint16> shadow_occlusion_without_bvh(const glsl::Cone& cone) const;
  QSet<quint16> shadow_occlusion_with_bvh(const glsl::Cone& cone, uint16_t root_node=0) const;
};

class VoxelBuffer
{
  // PERFORMANCE: really big voxelgrids (spinza itself don't need to be part of the bvh)
  // PERFORMANCE: sort also by size of the bounding spheres?
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
  typedef scene::Scene::Data::VoxelGrids VoxelGrid;

  glrt::scene::Scene& scene;
  VoxelGrid*& voxelGridData;

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
