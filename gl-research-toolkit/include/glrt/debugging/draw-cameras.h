#ifndef GLRT_DEBUGGING_DRAWCAMERAS_H
#define GLRT_DEBUGGING_DRAWCAMERAS_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glrt/scene/camera-parameter.h>
#include <glrt/toolkit/aligned-vector.h>

#include "debug-mesh.h"

namespace glrt {
namespace scene {

class Scene;

} // namespace scene

namespace debugging {


class DebugLineVisualisation final
{
public:
  typedef QSharedPointer<DebugLineVisualisation> Ptr;

  DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, int numDrawCalls, int bufferOffset);
  DebugLineVisualisation(DebugLineVisualisation&&);
  ~DebugLineVisualisation();

  template<typename UniformType>
  static DebugLineVisualisation debugRendering(const DebugMesh::Painter& painter, const QVector<UniformType>& uniformData);
  template<typename UniformType>
  static DebugLineVisualisation debugRendering(const DebugMesh::Painter& painter, const aligned_vector<UniformType>& uniformData);
  static DebugLineVisualisation drawCameras(const QVector<scene::CameraParameter>& sceneCameras);

  DebugLineVisualisation(const DebugLineVisualisation&) = delete;
  DebugLineVisualisation& operator=(const DebugLineVisualisation&) = delete;
  DebugLineVisualisation& operator=(DebugLineVisualisation&&) = delete;

  void draw();

private:
  gl::VertexArrayObject vertexArrayObject;
  DebugMesh debugMesh;
  gl::Buffer uniformBuffer;
  int numDrawCalls;
  int bufferOffset;
};

template<typename UniformType>
DebugLineVisualisation DebugLineVisualisation::debugRendering(const DebugMesh::Painter& painter, const QVector<UniformType>& uniformData)
{
  aligned_vector<UniformType> aligned_data(aligned_vector<UniformType>::Alignment::UniformBufferOffsetAlignment);
  aligned_data.reserve(uniformData.length());
  for(const UniformType& data : uniformData)
    aligned_data.push_back(data);

  return std::move(debugRendering(painter, aligned_data));
}

template<typename UniformType>
DebugLineVisualisation DebugLineVisualisation::debugRendering(const DebugMesh::Painter& painter, const aligned_vector<UniformType>& uniformData)
{
  return DebugLineVisualisation(std::move(painter.toMesh()),
                     gl::Buffer(uniformData.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, uniformData.data()),
                     uniformData.size(),
                     uniformData.alignment());
}

} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_DRAWCAMERAS_H
