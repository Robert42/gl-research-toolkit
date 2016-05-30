#ifndef GLRT_RENDERER_DEBUGGING_DRAWCAMERAS_H
#define GLRT_RENDERER_DEBUGGING_DRAWCAMERAS_H

#include <glrt/scene/camera-parameter.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/voxel-data-component.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/renderer/gl/program.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

#include "debug-mesh.h"

#include <glrt/renderer/debugging/debug-renderer.h>

namespace glrt {
namespace scene {

class Scene;

} // namespace scene

namespace renderer {
namespace debugging {

using scene::resources::VoxelBoundingBox;
using scene::resources::BoundingSphere;

struct Arrow
{
  glm::vec3 from;
  padding<float> _p1;
  glm::vec3 to;
  padding<float> _p2;
};


class DebugLineVisualisation final : public DebugRenderer::Implementation
{
public:
  DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, gl::Program&& glProgram, int numDrawCalls, int uniformBufferOffset, int uniformBufferElementSize);
  DebugLineVisualisation(DebugLineVisualisation&&);
  ~DebugLineVisualisation();

  template<typename UniformType>
  static DebugLineVisualisation debugRendering(const DebugMesh::Painter& painter, const QVector<UniformType>& uniformData, gl::Program&& glProgram);
  template<typename UniformType>
  static DebugLineVisualisation debugRendering(const DebugMesh::Painter& painter, const aligned_vector<UniformType>& uniformData, gl::Program&& glProgram);

  static DebugRenderer::Implementation* drawCameras(const QList<scene::CameraParameter>& sceneCameras);
  static DebugRenderer::Implementation* drawSphereAreaLights(const QList<scene::SphereAreaLightComponent::Data>& sphereAreaLights);
  static DebugRenderer::Implementation* drawRectAreaLights(const QList<scene::RectAreaLightComponent::Data>& rectAreaLights);
  static DebugRenderer::Implementation* drawPositions(const QVector<glm::vec3>& positions);
  static DebugRenderer::Implementation* drawArrows(const QVector<Arrow>& arrows);
  static DebugRenderer::Implementation* drawWorldGrid();
  static DebugRenderer::Implementation* drawVoxelGrids(const QList<VoxelBoundingBox>& gridSizes);
  static DebugRenderer::Implementation* drawSpheres(const QList<BoundingSphere>& spheres);


  DebugLineVisualisation(const DebugLineVisualisation&) = delete;
  DebugLineVisualisation& operator=(const DebugLineVisualisation&) = delete;
  DebugLineVisualisation& operator=(DebugLineVisualisation&&) = delete;

  void render() override;

private:
  gl::VertexArrayObject vertexArrayObject;
  DebugMesh debugMesh;
  gl::Buffer uniformBuffer;
  gl::Program glProgram;
  int numDrawCalls;
  int uniformBufferOffset;
  int uniformBufferElementSize;
  bool use_dephtest = false;
};

template<typename UniformType>
DebugLineVisualisation DebugLineVisualisation::debugRendering(const DebugMesh::Painter& painter, const QVector<UniformType>& uniformData, gl::Program&& glProgram)
{
  aligned_vector<UniformType> aligned_data(aligned_vector<UniformType>::Alignment::UniformBufferOffsetAlignment);
  aligned_data.reserve(uniformData.length());
  for(const UniformType& data : uniformData)
    aligned_data.push_back(data);

  return std::move(debugRendering(painter, aligned_data, std::move(glProgram)));
}

template<typename UniformType>
DebugLineVisualisation DebugLineVisualisation::debugRendering(const DebugMesh::Painter& painter, const aligned_vector<UniformType>& uniformData, gl::Program&& glProgram)
{
  return DebugLineVisualisation(std::move(painter.toMesh()),
                                gl::Buffer(uniformData.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, uniformData.data()),
                                std::move(glProgram),
                                uniformData.size(),
                                uniformData.alignment(),
                                sizeof(UniformType));
}


} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DRAWCAMERAS_H
