#ifndef GLRT_RENDERER_VOXELBUFFER_H
#define GLRT_RENDERER_VOXELBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/toolkit/managed-gl-buffer.h>
#include <glrt/renderer/compute-step.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/scene/scene-data.h>


namespace glrt {
namespace glsl {

struct Cone;

} // namespace glsl
namespace renderer {

typedef scene::resources::DirectDataCandidateGridArrayCell DirectDataCandidateGridArrayCell;


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
  const float huge_bvh_limit;

  BVH(const BoundingSphere* leaves_bounding_spheres, const VoxelGrids& voxelGridData);
  BVH(const BoundingSphere* leaves_bounding_spheres, const quint32* leaves_z_indices, quint16 num_leaves, float huge_bvh_limit=INFINITY);
  void updateTreeCPU(BoundingSphere* bvhInnerBoundingSpheres, InnerNode* bvhInnerNodes);

  quint16 findSplit(quint16 begin, quint16 end);
  quint32 zIndexDistance(quint16 a, quint16 b) const;
  float boundingSphereRadius(quint16 a, quint16 b) const;
  float boundingSphereRadius_heuristic(quint16 a, quint16 b) const;

private:
  BoundingSphere* bvhInnerBoundingSpheres = nullptr;
  BVH::InnerNode* bvhInnerNodes = nullptr;
  quint16 num_inner_nodes = 0;
  const quint16 capacity_inner_nodes;

  quint16 addInnerNode();

  quint16 calcDepth(quint16 root, InnerNode* bvhInnerNodes) const;

  SubTree generateHierarchy(quint16 begin, quint16 end);
  SubTree generateSingleElementHierarchy();
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
  typedef scene::resources::utilities::GlTexture GlTexture;

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

  // Candidate Grid
  struct CandidateGridHeader
  {
    GLuint64 textureRenderHandle = 0;
    GLuint64 _reservedForTiles = 0;
    GLuint64 fallbackSDF = 0;
    GLuint64 _padding = 0;

    glm::vec4 gridLocation;
    glm::vec4 fallbackSdfLocation;

    GLuint64 candidateBuffer = 0;
    GLuint64 _candidate_reservedForTiles = 0;
  };
  struct CandidateGrid
  {
    GlTexture texture;
    glm::uvec3 size;

    CandidateGridHeader calcCandidates(const scene::Scene* scene, ManagedGLBuffer<uint8_t>* candidateGridBuffer, float influence_radius);
    CandidateGridHeader calcCandidates(const scene::Scene* scene, ManagedGLBuffer<DirectDataCandidateGridArrayCell>* candidateGridBuffer, float influence_radius, const QVector<scene::resources::VoxelUniformDataBlock>& distanceFieldVoxelData_cpu, const QVector<BoundingSphere>& distanceFieldboundingSpheres_cpu);

  private:
    template<typename T>
    CandidateGridHeader calcCandidatesImplementation(const scene::Scene* scene, ManagedGLBuffer<T>* candidateGridBuffer, float influence_radius, const std::function<T(uint8_t)>& generate_data);
  };
  CandidateGridHeader candidateGridHeader;
  CandidateGrid candidateGrid;

  VoxelBuffer(scene::Scene& scene);
  ~VoxelBuffer();

  const VoxelHeader& updateVoxelHeader();

  void mergeStaticSDFs();

  bool need_merged_sdf() const {return dirty_merged_sdf;}

private:
  typedef scene::Scene::Data::VoxelGrids VoxelGrid;

  glrt::scene::Scene& scene;
  VoxelGrid*& voxelGridData;
  bool dirty_candidate_grid = true;;
  bool dirty_merged_sdf_texture_buffer = true;
  bool dirty_merged_sdf = true;
  bool _static_fade_with_fallback = true;
  void update_static_fade_with_fallback();

  QVector<scene::resources::VoxelUniformDataBlock> distanceFieldVoxelData_cpu;
  QVector<BoundingSphere> distanceFieldboundingSpheres_cpu;
  ManagedGLBuffer<scene::resources::VoxelUniformDataBlock> distanceFieldVoxelData;
  ManagedGLBuffer<BoundingSphere> distanceFieldboundingSpheres;


  ManagedGLBuffer<BoundingSphere> bvhInnerBoundingSpheres;
  ManagedGLBuffer<BVH::InnerNode> bvhInnerNodes;

  // LIMIT_255
  ManagedGLBuffer<uint8_t> candidateIndexBuffer;
  ManagedGLBuffer<DirectDataCandidateGridArrayCell> candidateDataBuffer;

  VoxelHeader _voxelHeader;

  struct FallbackSdf
  {
    GlTexture texture;
    GLuint64 textureHandle;
    glm::vec4 location;
    glm::ivec3 resolution;
  };
  FallbackSdf staticFallbackSdf;
  DynamicComputeStep mergeSDFs;

  void updateVoxelGrid();
  void updateBvhTree(const glrt::scene::resources::BoundingSphere* leaves_bounding_spheres);

  gl::Buffer sdfMergeHeaderBuffer;
  void initStaticSDFMerged();
  void initStaticSdfFallbackTexture();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
