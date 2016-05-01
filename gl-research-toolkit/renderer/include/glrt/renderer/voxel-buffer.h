#ifndef GLRT_RENDERER_VOXELBUFFER_H
#define GLRT_RENDERER_VOXELBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/simple-shader-storage-buffer.h>


namespace glrt {
namespace renderer {

class VoxelBuffer
{
public:
  struct VoxelHeader
  {
    GLuint64 distanceFieldWorldToVoxelMatrices;
    GLuint64 distanceFieldGridSizes;
    GLuint64 distanceFieldSpaceFactors;
    GLuint64 distanceFieldTextures;
    quint32 numDistanceFields;
    padding<quint32, 3> _padding;
  };

  VoxelBuffer(scene::Scene& scene);
  ~VoxelBuffer();

  const VoxelHeader& updateVoxelHeader();

private:
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, glm::mat4, &scene::VoxelDataComponent::globalWorldToVoxelMatrix>> distanceFieldMat4StorageBuffer;
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, scene::VoxelDataComponent::GridSize, &scene::VoxelDataComponent::gridSize>> distanceFieldGridSizesStorageBuffer;
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, scene::VoxelDataComponent::WorldVoxelUvwSpaceFactor, &scene::VoxelDataComponent::spaceFactor>> distanceFieldSpaceFactorStorageBuffer;
  SimpleShaderStorageBuffer<scene::VoxelDataComponent, implementation::RandomComponentDataDescription<scene::VoxelDataComponent, quint64, &scene::VoxelDataComponent::textureData>> distanceFieldTextureHandleStorageBuffer;

  VoxelHeader _voxelHeader;

  quint32 numVisibleVoxelGrids() const;

  quint32 _numVisibleVoxelGrids = 0;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
