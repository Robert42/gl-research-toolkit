#include <glrt/renderer/voxel-buffer.h>
#include <glrt/renderer/bvh-usage.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/scene/resources/voxelizer.h>
#include <glrt/toolkit/profiler.h>
#include <glrt/toolkit/zindex.h>
#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {

// ==========================================================================================================================================================

// ==== VoxelBuffer =========================================================================================================================================

constexpr const uint32_t item_capacity = MAX_SDF_CANDIDATE_GRID_SIZE*MAX_SDF_CANDIDATE_GRID_SIZE*MAX_SDF_CANDIDATE_GRID_SIZE*255;

VoxelBuffer::VoxelBuffer(glrt::scene::Scene& scene)
  : scene(scene),
    voxelGridData(scene.data->voxelGrids),
    distanceFieldVoxelData(scene.data->voxelGrids->capacity()),
    distanceFieldboundingSpheres(scene.data->voxelGrids->capacity()),
    bvhInnerBoundingSpheres(scene.data->voxelGrids->capacity()),
    bvhInnerNodes(scene.data->voxelGrids->capacity()),
    // LIMIT_255
    candidateIndexBuffer(item_capacity),
    candidateDataBuffer(item_capacity),
    mergeSDFs(GLRT_SHADER_DIR "/compute/merge-sdf.cs",
              glm::uvec3(16))
{
  distanceFieldVoxelData_cpu.resize(item_capacity);
  distanceFieldboundingSpheres_cpu.resize(item_capacity);

  dirty_candidate_grid = true;
  dirty_merged_sdf_texture_buffer = true;
  dirty_merged_sdf = true;
  AO_RADIUS.callback_functions << [this](float){dirty_candidate_grid=true;};

  MERGED_STATIC_SDF_SIZE.callback_functions << [this](uint32_t){dirty_merged_sdf_texture_buffer=true;};

  AO_IGNORE_FALLBACK_SDF.callback_functions << [this](uint32_t){update_static_fade_with_fallback();};
  AO_FALLBACK_SDF_ONLY.callback_functions << [this](uint32_t){update_static_fade_with_fallback();};
  AO_STATIC_FALLBACK_FADING_END.callback_functions << [this](uint32_t){dirty_candidate_grid = true;};
  AO_RADIUS.callback_functions << [this](uint32_t){dirty_candidate_grid = true;};
  AO_CANDIDATE_GRID_CONTAINS_INDICES.callback_functions << [this](uint32_t){dirty_candidate_grid = true;};

  initStaticSDFMerged();
}

VoxelBuffer::~VoxelBuffer()
{
}

const VoxelBuffer::VoxelHeader& VoxelBuffer::updateVoxelHeader()
{
  PROFILE_SCOPE("VoxelBuffer::updateVoxelHeader()")

  // TODO: (voxelGridData->numDynamic>0) is not an elegant solution for dynamic objects
  if(Q_UNLIKELY(voxelGridData->numDynamic>0 || voxelGridData->dirtyOrder))
  {
    dirty_candidate_grid = true;
    dirty_merged_sdf_texture_buffer = true;
    updateVoxelGrid();

    _voxelHeader.numDistanceFields = voxelGridData->length;
    _voxelHeader.distanceFieldBvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.buffer.gpuBufferAddress();
    _voxelHeader.distanceFieldBvhInnerNodes = this->bvhInnerNodes.buffer.gpuBufferAddress();
    _voxelHeader.distanceFieldDataStorageBuffer = this->distanceFieldVoxelData.gpuBufferAddress();
    _voxelHeader.distanceFieldBoundingSpheres = this->distanceFieldboundingSpheres.gpuBufferAddress();

    Q_ASSERT(voxelGridData->dirtyOrder == false);
  }

  if(Q_UNLIKELY(dirty_merged_sdf_texture_buffer))
  {
    initStaticSdfFallbackTexture();

    dirty_merged_sdf_texture_buffer = false;
    dirty_merged_sdf = true;

    candidateGridHeader.fallbackSDF = staticFallbackSdf.textureHandle;
    candidateGridHeader.fallbackSdfLocation = staticFallbackSdf.location;
  }

  if(Q_UNLIKELY(dirty_candidate_grid))
  {
    const float radius = _static_fade_with_fallback ? AO_STATIC_FALLBACK_FADING_END : AO_RADIUS;

    if(AO_CANDIDATE_GRID_CONTAINS_INDICES)
      candidateGridHeader = candidateGrid.calcCandidates(&scene, &candidateIndexBuffer, radius);
    else
      candidateGridHeader = candidateGrid.calcCandidates(&scene, &candidateDataBuffer, radius, distanceFieldVoxelData_cpu, distanceFieldboundingSpheres_cpu);
    candidateGridHeader.fallbackSDF = staticFallbackSdf.textureHandle;
    candidateGridHeader.fallbackSdfLocation = staticFallbackSdf.location;

    dirty_candidate_grid = false;
  }

  return _voxelHeader;
}

void VoxelBuffer::updateVoxelGrid()
{
  PROFILE_SCOPE("VoxelBuffer::updateVoxelGrid()")

  const quint16 n = voxelGridData->length;

  scene::AABB scene_aabb = scene::AABB::invalid();
#pragma omp simd
  for(quint16 i=0; i<n; ++i)
    voxelGridData->aabb_for(&scene_aabb, i);

  quint32* z_indices = voxelGridData->z_index;
#pragma omp simd
  for(quint16 i=0; i<n; ++i)
    z_indices[i] = voxelGridData->z_index[i] = calcZIndex(scene_aabb.toUnitSpace(voxelGridData->position[i]));

  scene.aabb = scene_aabb;
  voxelGridData->huge_bvh_limit = scene::BVH_HUGE_LEAVES_LIMIT * glm::distance(scene_aabb.maxPoint, scene_aabb.minPoint);

  // -- W A R N I N G -- voxelgriddata swap happening here!!!!
  scene.data->sort_voxelGrids();

  // after swapping the old address is invalid
  z_indices = voxelGridData->z_index;
  const float* scaleFactor = voxelGridData->scaleFactor;
  const BoundingSphere* localSpaceBoundingSpheres = voxelGridData->boundingSphere;

  const quint16 n_mintwo = glm::max<quint16>(2, n);

  scene::resources::VoxelUniformDataBlock* const dataBlock = distanceFieldVoxelData_cpu.data();
  BoundingSphere* const boundingSphere = distanceFieldboundingSpheres_cpu.data();

  #pragma omp simd
  for(quint16 i=0; i<n; ++i)
  {
    scene::CoordFrame globalCoordFrame = voxelGridData->globalCoordFrame(i);

    const scene::resources::VoxelData& data = voxelGridData->voxelData[i];
    dataBlock[i].globalWorldToVoxelFactor = data.localToVoxelSpace.scaleFactor / scaleFactor[i];
    const glm::mat4x3 globalWorldToVoxelMatrix = data.worldToVoxelSpaceMatrix4x3(globalCoordFrame);
    const glm::ivec3 voxelCount = data.voxelCount;
    const quint64 gpuTextureHandle = data.gpuTextureHandle;

    dataBlock[i].globalWorldToVoxelMatrix_col0 = globalWorldToVoxelMatrix[0];
    dataBlock[i].globalWorldToVoxelMatrix_col1 = globalWorldToVoxelMatrix[1];
    dataBlock[i].globalWorldToVoxelMatrix_col2 = globalWorldToVoxelMatrix[2];
    dataBlock[i].globalWorldToVoxelMatrix_col3 = globalWorldToVoxelMatrix[3];
    dataBlock[i].voxelCount_x = voxelCount.x;
    dataBlock[i].voxelCount_y = voxelCount.y;
    dataBlock[i].voxelCount_z = voxelCount.z;
    dataBlock[i].texture = gpuTextureHandle;

    boundingSphere[i] = globalCoordFrame * localSpaceBoundingSpheres[i];
  }

  if(Q_UNLIKELY(n==1))
  {
    dataBlock[1] = dataBlock[0];
    boundingSphere[1] = BoundingSphere{glm::vec3(NAN), NAN};
  }

  updateBvhTree(boundingSphere);

  memcpy(distanceFieldVoxelData.Map(n_mintwo), dataBlock, size_t(n_mintwo) * sizeof(scene::resources::VoxelUniformDataBlock));
  distanceFieldVoxelData.Unmap();

  memcpy(distanceFieldboundingSpheres.Map(n_mintwo), boundingSphere, size_t(n_mintwo) * sizeof(BoundingSphere));
  distanceFieldboundingSpheres.Unmap();
}

void VoxelBuffer::updateBvhTree(const BoundingSphere* leaves_bounding_spheres)
{
  PROFILE_SCOPE("VoxelBuffer::updateBvhTree()")

  const quint16 numElements = voxelGridData->length;
  const quint16 numInnerNodes = glm::max<quint16>(1, numElements - 1);

  Q_ASSERT(numElements>0);

  BVH bvh(leaves_bounding_spheres, *voxelGridData);

  BoundingSphere* bvhInnerBoundingSpheres = this->bvhInnerBoundingSpheres.Map(numInnerNodes);
  BVH::InnerNode* bvhInnerNodes = this->bvhInnerNodes.Map(numInnerNodes);
  bvh.updateTreeCPU(bvhInnerBoundingSpheres, bvhInnerNodes);
  this->bvhInnerBoundingSpheres.Unmap();
  this->bvhInnerNodes.Unmap();

}





// ==========================================================================================================================================================

// ======== CandidateGrid ===================================================================================================================================


using scene::resources::Voxelizer;
using scene::resources::VoxelGridGeometry;

Array<uint16_t> collectAllSdfIntersectingWith(const scene::Scene::Data* data, const glm::uvec3 voxel, const VoxelGridGeometry& geometry, float influence_radius);

VoxelBuffer::CandidateGridHeader VoxelBuffer::CandidateGrid::calcCandidates(const scene::Scene* scene, ManagedGLBuffer<uint8_t>* candidateGridBuffer, float influence_radius)
{
  PROFILE_SCOPE("CandidateGrid::calcCandidateGrid(uint8_t*)");

  return calcCandidatesImplementation<uint8_t>(scene, candidateGridBuffer, influence_radius, [](uint8_t i){return i;});
}

VoxelBuffer::CandidateGridHeader VoxelBuffer::CandidateGrid::calcCandidates(const scene::Scene* scene, ManagedGLBuffer<DirectDataCandidateGridArrayCell>* candidateGridBuffer, float influence_radius, const QVector<scene::resources::VoxelUniformDataBlock>& distanceFieldVoxelData_cpu, const QVector<BoundingSphere>& distanceFieldboundingSpheres_cpu)
{
  PROFILE_SCOPE("CandidateGrid::calcCandidateGrid(DirectDataCandidateGridArrayCell*)");

  const scene::Scene::Data* data = scene->data;

  const scene::resources::VoxelUniformDataBlock* distanceFieldVoxelData = distanceFieldVoxelData_cpu.data();
  const BoundingSphere* distanceFieldboundingSpheres = distanceFieldboundingSpheres_cpu.data();
  CandidateGridHeader header = calcCandidatesImplementation<DirectDataCandidateGridArrayCell>(scene,
                                                                        candidateGridBuffer,
                                                                        influence_radius,
                                                                        [data, distanceFieldVoxelData, distanceFieldboundingSpheres](uint8_t i){
    DirectDataCandidateGridArrayCell c;
    c.boundingSphere = distanceFieldboundingSpheres[i];
    c.block = distanceFieldVoxelData[i];
    return c;
  });
  return header;
}

template<typename T>
VoxelBuffer::CandidateGridHeader VoxelBuffer::CandidateGrid::calcCandidatesImplementation(const scene::Scene* scene, ManagedGLBuffer<T>* candidateGridBuffer, float influence_radius, const std::function<T(uint8_t)>& generate_data)
{

  const scene::Scene::Data* data = scene->data;
  // TODO: seperate aabb for whole scene and for static scene?
  const scene::AABB aabb = scene->aabb.ensureValid();

  VoxelGridGeometry geometry = Voxelizer::calcVoxelSize(aabb, int(SDF_CANDIDATE_GRID_SIZE), true);

  glm::uvec3 size = geometry.gridSize;

  Q_ASSERT(all(greaterThan(size, glm::uvec3(0))));

  // The current format uses a single integer. The lower 24 bits store the byte
  // offset from the start of the buffer to the sdf indices stored by this grid.
  // LIMIT_255
  Q_ASSERT(data->voxelGrids->length <= 255);
  Q_ASSERT(size.x*size.y*size.z*255 <= 0x01000000);

  auto format = GlTexture::format(size, 0, GlTexture::Format::RED_INTEGER, GlTexture::Type::UINT32, GlTexture::Target::TEXTURE_3D);

  QVector<uint32_t> _buffer;
  Array<Array<uint16_t>> _collectedSDFs;

  _collectedSDFs.resize_memset_zero(int(size.x*size.y*size.z));
  _buffer.resize(int(size.x*size.y*size.z));

  for(Array<uint16_t>& a : _collectedSDFs)
    a.reserve(data->voxelGrids->length);

  uint32_t* const buffer = _buffer.data();
  Array<uint16_t>* const collectedSDFs = _collectedSDFs.data();

  const glm::vec4 gridLocation(geometry.toVoxelSpace.position, geometry.toVoxelSpace.scaleFactor);

#pragma omp parallel for
  for(uint32_t x=0; x<size.x; x++)
  {
    for(uint32_t y=0; y<size.y; y++)
    {
      for(uint32_t z=0; z<size.z; z++)
      {
        uint32_t offset = voxelIndexForCoordinate(glm::uvec3(x, y, z), size);
        glm::uvec3 voxelCoord(x,y,z);
        Array<uint16_t>& sdfs = *(collectedSDFs + offset);

        sdfs = collectAllSdfIntersectingWith(data, voxelCoord, geometry, influence_radius);

        Q_ASSERT(sdfs.length() <= 255);

      }
    }
  }

  uint32_t data_offset = 0;

  T* candidate_data_buffer = candidateGridBuffer->Map();
  for(uint32_t x=0; x<size.x; x++)
  {
    for(uint32_t y=0; y<size.y; y++)
    {
      for(uint32_t z=0; z<size.z; z++)
      {
        uint32_t offset = voxelIndexForCoordinate(glm::uvec3(x, y, z), size);
        Array<uint16_t>& sdfs = *(collectedSDFs + offset);
        const uint32_t num_candidates = uint32_t(sdfs.length());
        uint32_t& voxelData = *(buffer + offset);
        T* candidate_data = reinterpret_cast<T*>(reinterpret_cast<T*>(candidate_data_buffer) + data_offset);
        Q_ASSERT(num_candidates <= 255);
        Q_ASSERT(data_offset <= 0x00ffffff);
        Q_ASSERT(data_offset+num_candidates <= 0x01000000);

        for(uint32_t i=0; i<num_candidates; ++i)
        {
          uint16_t index = sdfs[int(i)];
          Q_ASSERT(index <= 255);
          candidate_data[i] = generate_data(static_cast<uint8_t>(index));
        }

        voxelData = (num_candidates <<  24) | (data_offset);

        data_offset += num_candidates;
      }
    }
  }
  candidateGridBuffer->Unmap();

#define ALWAYS_NEW_TEXTURE_OBJECT 1

#if !ALWAYS_NEW_TEXTURE_OBJECT
  if(this->size != size)
  {
#endif
    this->size = size;
    texture = GlTexture();
#if !ALWAYS_NEW_TEXTURE_OBJECT
  }else if(textureRenderHandle!=0)
  {
    if(GL_RET_CALL(glIsTextureHandleResidentNV, textureRenderHandle))
      GL_CALL(glMakeTextureHandleNonResidentNV, textureRenderHandle);
  }
#endif

  CandidateGridHeader header;

  texture.setUncompressed2DImage(format, buffer);
  texture.makeComplete();
  header.textureRenderHandle = GL_RET_CALL(glGetTextureHandleNV, texture.textureId);
  GL_CALL(glMakeTextureHandleResidentNV, header.textureRenderHandle);

  header.gridLocation = gridLocation;
  header.candidateBuffer = candidateGridBuffer->gpuBufferAddress();

  debugging::VisualizationRenderer::setSdfCandidateGridData(std::move(_collectedSDFs));

  return header;
}

struct SDFMergeHeader
{
  GLuint64 targetTexture;
  padding<GLuint64, 1> _padding1;
  glm::vec4 gridLocation;
  glm::uvec3 targetTextureSize;
  padding<uint, 1> _padding3;
};

void VoxelBuffer::initStaticSDFMerged()
{
  sdfMergeHeaderBuffer = gl::Buffer(sizeof(SDFMergeHeader), gl::Buffer::MAP_WRITE);
}


void VoxelBuffer::initStaticSdfFallbackTexture()
{
  PROFILE_SCOPE("CandidateGrid::mergeStaticSDFs");
  // TODO: seperate aabb for whole scene and for static scene?
  const scene::AABB aabb = scene.aabb.ensureValid();

  SDFMergeHeader& sdfMergeHeader = *reinterpret_cast<SDFMergeHeader*>(sdfMergeHeaderBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  VoxelGridGeometry geometry = Voxelizer::calcVoxelSize(aabb, int(MERGED_STATIC_SDF_SIZE), true);


  if(staticFallbackSdf.resolution != geometry.gridSize)
  {
    staticFallbackSdf.resolution = geometry.gridSize;
    staticFallbackSdf.texture = GlTexture();
    staticFallbackSdf.texture.setUncompressed2DImage(GlTexture::format(glm::uvec3(staticFallbackSdf.resolution),
                                                                       0,
                                                                       GlTexture::Format::RED,
                                                                       GlTexture::Type::FLOAT16,
                                                                       GlTexture::Target::TEXTURE_3D), nullptr);
    staticFallbackSdf.texture.makeComplete();
    staticFallbackSdf.textureHandle = GL_RET_CALL(glGetTextureHandleNV, staticFallbackSdf.texture.textureId);
    GL_CALL(glMakeTextureHandleResidentNV, staticFallbackSdf.textureHandle);
  }

  sdfMergeHeader.gridLocation = glm::vec4(geometry.toVoxelSpace.position, geometry.toVoxelSpace.scaleFactor);

  staticFallbackSdf.location = sdfMergeHeader.gridLocation;

  GLuint64 imageHandle = GL_RET_CALL(glGetImageHandleNV, staticFallbackSdf.texture.textureId, 0, GL_TRUE, 0, GL_R16F);
  if(!GL_RET_CALL(glIsImageHandleResidentNV, imageHandle))
    GL_CALL(glMakeImageHandleResidentNV, imageHandle, GL_WRITE_ONLY);

  sdfMergeHeader.targetTexture = imageHandle;
  sdfMergeHeader.targetTextureSize = glm::uvec3(MERGED_STATIC_SDF_SIZE);

  sdfMergeHeaderBuffer.Unmap();

}

void glrt::renderer::VoxelBuffer::mergeStaticSDFs()
{
  dirty_merged_sdf = false;

  sdfMergeHeaderBuffer.BindUniformBuffer(UNIFORM_MERGE_SDF_BLOCK);

  mergeSDFs.invoke(glm::uvec3(MERGED_STATIC_SDF_SIZE));
}

void VoxelBuffer::update_static_fade_with_fallback()
{
  bool should_fade = !AO_FALLBACK_SDF_ONLY && !AO_IGNORE_FALLBACK_SDF;
  if(_static_fade_with_fallback != should_fade)
  {
    _static_fade_with_fallback = should_fade;
    dirty_candidate_grid = true;
  }
}




// ==========================================================================================================================================================

// ======== BVH =============================================================================================================================================

BVH::BVH(const BoundingSphere* leaves_bounding_spheres, const VoxelGrids& voxelGridData)
  : BVH(leaves_bounding_spheres, voxelGridData.z_index, voxelGridData.length, voxelGridData.huge_bvh_limit)
{
}

BVH::BVH(const BoundingSphere* leaves_bounding_spheres, const quint32* leaves_z_indices, quint16 num_leaves, float huge_bvh_limit)
  : leaves_bounding_spheres(leaves_bounding_spheres),
    leaves_z_indices(leaves_z_indices),
    num_leaves(num_leaves),
    huge_bvh_limit(huge_bvh_limit),
    capacity_inner_nodes(num_leaves)
{
}

void BVH::updateTreeCPU(BoundingSphere* bvhInnerBoundingSpheres, BVH::InnerNode* bvhInnerNodes)
{
  this->bvhInnerBoundingSpheres = bvhInnerBoundingSpheres;
  this->bvhInnerNodes = bvhInnerNodes;

  Q_ASSERT(num_leaves>0);

  // Make sure, the last inner Node has some valid values, so a branchless tree traversal can assume,it's never NAN
  this->bvhInnerBoundingSpheres[num_leaves-1] = BoundingSphere{glm::vec3(10000.f), 0.f};
  this->bvhInnerNodes[num_leaves-1].left_child = 0x8000;
  this->bvhInnerNodes[num_leaves-1].right_child = 0x8000;

  const quint16 skip_first_n = scene::HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE;
  if(Q_LIKELY(num_leaves > 1 + skip_first_n))
  {
    generateHierarchy(skip_first_n, num_leaves);
    verifyHierarchy();
    testOcclusion();
  }else
  {
    generateSingleElementHierarchy();
  }


  this->bvhInnerBoundingSpheres = nullptr;
  this->bvhInnerNodes = nullptr;
}

void BVH::verifyHierarchy() const
{
  verifyTreeDepth();
  verifyBoundingSpheres();
}

void BVH::verifyTreeDepth() const
{
  const quint16 depth = calcDepth(0, bvhInnerNodes);
  if(Q_UNLIKELY(depth == num_leaves-1))
    qWarning() << "Malformed tree (no performence gains)";
  if(Q_UNLIKELY(bvh_traversal_stack_depth() < depth))
    qWarning() << "bvh has a greater depth than the bvh traversal stack";
  qDebug() << "BVH::updateTreeCPU{num_leaves:" <<num_leaves << " depth:"<<depth<<"}";
  Q_ASSERT(depth < MAX_NUM_STATIC_MESHES);
}

void BVH::verifyBoundingSpheres(uint16_t root_node) const
{
  const uint16_t* inner_nodes = reinterpret_cast<const uint16_t*>(this->bvhInnerNodes);

  uint16_t stack[MAX_NUM_STATIC_MESHES];
  stack[0] = root_node;
  uint16_t stack_depth=uint16_t(1);

  do {
    stack_depth--;
    uint16_t current_node = stack[stack_depth];
    const BoundingSphere& currentBoundingSphere = bvhInnerBoundingSpheres[current_node];

    const uint16_t* child_nodes = inner_nodes + current_node*uint16_t(2);
    uint16_t left_node = child_nodes[0];
    uint16_t right_node = child_nodes[1];

    bool left_is_inner_node = (left_node & uint16_t(0x8000)) == uint16_t(0);
    bool right_is_inner_node = (right_node & uint16_t(0x8000)) == uint16_t(0);
    left_node = left_node & uint16_t(0x7fff);
    right_node = right_node & uint16_t(0x7fff);

    const float epsilon = 0.f;

    if(left_is_inner_node)
    {
      Q_ASSERT(currentBoundingSphere.contains(bvhInnerBoundingSpheres[left_node], epsilon));
      stack[stack_depth++] = left_node;
    }else
    {
      Q_ASSERT(currentBoundingSphere.contains(leaves_bounding_spheres[left_node], epsilon));
    }

    if(right_is_inner_node)
    {
      Q_ASSERT(currentBoundingSphere.contains(bvhInnerBoundingSpheres[right_node], epsilon));
      stack[stack_depth++] = right_node;
    }else
    {
      Q_ASSERT(currentBoundingSphere.contains(leaves_bounding_spheres[right_node], epsilon));
    }

  }while(stack_depth>uint16_t(0));
}


quint16 BVH::addInnerNode()
{
  Q_ASSERT(num_inner_nodes < capacity_inner_nodes);
  return num_inner_nodes++;
}

quint16 BVH::calcDepth(quint16 root, InnerNode* bvhInnerNodes) const
{
  if(scene::ENFORCE_HUGE_BVH_LEAVES_FIRST)
  {
    Q_ASSERT(root!=0x8000);
  }

  if(root & 0x8000)
    return 0;

  Q_ASSERT(num_inner_nodes > root);

  return 1+glm::max(calcDepth(bvhInnerNodes[root].left_child, bvhInnerNodes),
                    calcDepth(bvhInnerNodes[root].right_child, bvhInnerNodes));
}


} // namespace renderer
} // namespace glrt
