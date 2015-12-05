#include <glrt/debugging/draw-cameras.h>

#include <glrt/scene/scene.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace debugging {


DrawCameras::DrawCameras(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, int numDrawCalls, int bufferOffset)
  : vertexArrayObject(DebugMesh::generateVertexArrayObject()),
    debugMesh(std::move(debugMesh)),
    uniformBuffer(std::move(uniformBuffer)),
    numDrawCalls(numDrawCalls),
    bufferOffset(bufferOffset)
{
}

DrawCameras::DrawCameras(DrawCameras&& drawCameras)
  : vertexArrayObject(std::move(drawCameras.vertexArrayObject)),
    debugMesh(std::move(drawCameras.debugMesh)),
    uniformBuffer(std::move(drawCameras.uniformBuffer)),
    numDrawCalls(drawCameras.numDrawCalls),
    bufferOffset(drawCameras.bufferOffset)
{
  drawCameras.numDrawCalls = 0;
  drawCameras.bufferOffset = 0;
}

DrawCameras::~DrawCameras()
{
}


DrawCameras DrawCameras::drawCameras(const QVector<scene::CameraParameter>& sceneCameras)
{
  DebugMesh::Painter painter;

  painter.addSphere(1.f, 16);

  return debugRendering(painter, sceneCameras);
}


void DrawCameras::draw()
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

