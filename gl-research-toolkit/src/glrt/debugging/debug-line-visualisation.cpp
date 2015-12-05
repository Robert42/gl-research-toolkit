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

  float cameraLength = 0.25;
  float cameraWidth = 0.1;
  float cameraHeight = 0.17;
  float cameraCylinderRadius = cameraLength*0.5f;

  painter.addCube(glm::vec3(-.5f*cameraWidth, -.5f*cameraHeight, 0), glm::vec3(.5f*cameraWidth, .5f*cameraHeight, cameraLength));

  painter.pushMatrix(glm::vec3(0, cameraHeight*0.5+cameraCylinderRadius, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
  painter.addCylinder(cameraCylinderRadius, cameraWidth, 16);
  painter.pushMatrix(glm::vec3(0, cameraCylinderRadius*2, 0));
  painter.addCylinder(cameraCylinderRadius, cameraWidth, 16);
  painter.popMatrix();
  painter.popMatrix();

  painter.nextAttribute.parameter1 = 1.f;
  painter.addCube(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 1));

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

