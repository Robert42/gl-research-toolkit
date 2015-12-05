#include <glrt/debugging/draw-cameras.h>

#include <glrt/scene/scene.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace debugging {


DebugLineVisualisation::DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, int numDrawCalls, int bufferOffset)
  : vertexArrayObject(DebugMesh::generateVertexArrayObject()),
    debugMesh(std::move(debugMesh)),
    uniformBuffer(std::move(uniformBuffer)),
    numDrawCalls(numDrawCalls),
    bufferOffset(bufferOffset)
{
}

DebugLineVisualisation::DebugLineVisualisation(DebugLineVisualisation&& other)
  : vertexArrayObject(std::move(other.vertexArrayObject)),
    debugMesh(std::move(other.debugMesh)),
    uniformBuffer(std::move(other.uniformBuffer)),
    numDrawCalls(other.numDrawCalls),
    bufferOffset(other.bufferOffset)
{
  other.numDrawCalls = 0;
  other.bufferOffset = 0;
}

DebugLineVisualisation::~DebugLineVisualisation()
{
}


DebugLineVisualisation DebugLineVisualisation::drawCameras(const QVector<scene::CameraParameter>& sceneCameras)
{
  DebugMesh::Painter painter;

  painter.addSphere(1.f, 16);

  return debugRendering(painter, sceneCameras);
}


void DebugLineVisualisation::draw()
{
  vertexArrayObject.Bind();
  debugMesh.bind(this->vertexArrayObject);

  for(int i=0; i<numDrawCalls; ++i)
  {
    uniformBuffer.BindUniformBuffer(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, i*bufferOffset, vertexArrayObject.GetVertexStride(0));
    debugMesh.draw();
  }

  vertexArrayObject.ResetBinding();
}


} // namespace debugging
} // namespace glrt

