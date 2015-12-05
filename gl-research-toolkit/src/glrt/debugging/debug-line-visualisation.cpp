#include <glrt/debugging/debug-line-visualisation.h>

#include <glrt/scene/scene.h>
#include <glrt/glsl/layout-constants.h>
#include <glrt/toolkit/shader-compiler.h>

namespace glrt {
namespace debugging {


DebugLineVisualisation::DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, gl::ShaderObject&& shaderObject, int numDrawCalls, int bufferOffset)
  : vertexArrayObject(DebugMesh::generateVertexArrayObject()),
    debugMesh(std::move(debugMesh)),
    uniformBuffer(std::move(uniformBuffer)),
    shaderObject(std::move(shaderObject)),
    numDrawCalls(numDrawCalls),
    bufferOffset(bufferOffset)
{
}

DebugLineVisualisation::DebugLineVisualisation(DebugLineVisualisation&& other)
  : vertexArrayObject(std::move(other.vertexArrayObject)),
    debugMesh(std::move(other.debugMesh)),
    uniformBuffer(std::move(other.uniformBuffer)),
    shaderObject(std::move(other.shaderObject)),
    numDrawCalls(other.numDrawCalls),
    bufferOffset(other.bufferOffset)
{
  other.numDrawCalls = 0;
  other.bufferOffset = 0;
}

DebugLineVisualisation::~DebugLineVisualisation()
{
}


DebugLineVisualisation::Ptr DebugLineVisualisation::drawCameras(const QVector<scene::CameraParameter>& sceneCameras)
{
  DebugMesh::Painter painter;

  painter.addArrow(0.75f, 0.1f);

  return Ptr(new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                 sceneCameras,
                                                                 std::move(ShaderCompiler::createShaderFromFiles("visualize-scene-camera",
                                                                                                                 QDir(GLRT_SHADER_DIR"/debugging/visualizations")))))));
}


void DebugLineVisualisation::draw()
{
  shaderObject.Activate();

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

