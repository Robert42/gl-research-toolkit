#ifndef GLRT_DEBUGGING_DRAWCAMERAS_H
#define GLRT_DEBUGGING_DRAWCAMERAS_H

#include <glrt/scene/camera-parameter.h>
#include <glrt/scene/light-component.h>
#include <glrt/toolkit/aligned-vector.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

#include "debug-mesh.h"

namespace glrt {
namespace scene {

class Scene;

} // namespace scene

namespace debugging {



struct Arrow
{
  glm::vec3 from;
  padding<float> _p1;
  glm::vec3 to;
  padding<float> _p2;
};


class DebugLineVisualisation final
{
public:
  typedef QSharedPointer<DebugLineVisualisation> Ptr;

  DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, gl::ShaderObject&& shaderObject, int numDrawCalls, int uniformBufferOffset, int uniformBufferElementSize);
  DebugLineVisualisation(DebugLineVisualisation&&);
  ~DebugLineVisualisation();

  template<typename UniformType>
  static DebugLineVisualisation debugRendering(const DebugMesh::Painter& painter, const QVector<UniformType>& uniformData, gl::ShaderObject&& shaderObject);
  template<typename UniformType>
  static DebugLineVisualisation debugRendering(const DebugMesh::Painter& painter, const aligned_vector<UniformType>& uniformData, gl::ShaderObject&& shaderObject);

  static Ptr drawCameras(const QList<scene::CameraParameter>& sceneCameras);
  static Ptr drawSphereAreaLights(const QList<scene::SphereAreaLightComponent::Data>& sphereAreaLights);
  static Ptr drawRectAreaLights(const QList<scene::RectAreaLightComponent::Data>& rectAreaLights);
  static Ptr drawPositions(const QVector<glm::vec3>& positions);
  static Ptr drawArrows(const QVector<Arrow>& arrows);

  DebugLineVisualisation(const DebugLineVisualisation&) = delete;
  DebugLineVisualisation& operator=(const DebugLineVisualisation&) = delete;
  DebugLineVisualisation& operator=(DebugLineVisualisation&&) = delete;

  void draw();

private:
  gl::VertexArrayObject vertexArrayObject;
  DebugMesh debugMesh;
  gl::Buffer uniformBuffer;
  gl::ShaderObject shaderObject;
  int numDrawCalls;
  int uniformBufferOffset;
  int uniformBufferElementSize;
};

template<typename UniformType>
DebugLineVisualisation DebugLineVisualisation::debugRendering(const DebugMesh::Painter& painter, const QVector<UniformType>& uniformData, gl::ShaderObject&& shaderObject)
{
  aligned_vector<UniformType> aligned_data(aligned_vector<UniformType>::Alignment::UniformBufferOffsetAlignment);
  aligned_data.reserve(uniformData.length());
  for(const UniformType& data : uniformData)
    aligned_data.push_back(data);

  return std::move(debugRendering(painter, aligned_data, std::move(shaderObject)));
}

template<typename UniformType>
DebugLineVisualisation DebugLineVisualisation::debugRendering(const DebugMesh::Painter& painter, const aligned_vector<UniformType>& uniformData, gl::ShaderObject&& shaderObject)
{
  return DebugLineVisualisation(std::move(painter.toMesh()),
                                gl::Buffer(uniformData.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, uniformData.data()),
                                std::move(shaderObject),
                                uniformData.size(),
                                uniformData.alignment(),
                                sizeof(UniformType));
}


} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_DRAWCAMERAS_H
