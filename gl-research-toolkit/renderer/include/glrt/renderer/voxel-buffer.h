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
    GLuint64 distanceFieldDataHeaders;
    quint32 numDistanceFields;
  };

  VoxelBuffer(scene::Scene& scene);
  ~VoxelBuffer();

  const VoxelHeader& updateVoxelHeader();

private:
  SimpleShaderStorageBuffer<scene::VoxelDataComponent> voxelDataStorageBuffer;

  VoxelHeader _voxelHeader;

  quint32 numVisibleVoxelGrids() const {return static_cast<quint32>(voxelDataStorageBuffer.numElements());}

  quint32 _numVisibleVoxelGrids = 0;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELBUFFER_H
