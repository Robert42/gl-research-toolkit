#include <glrt/debugging/debug-line-visualisation.h>

#include <glrt/scene/scene.h>
#include <glrt/glsl/layout-constants.h>
#include <glrt/toolkit/shader-compiler.h>

namespace glrt {
namespace debugging {


DebugLineVisualisation::DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, gl::ShaderObject&& shaderObject, int numDrawCalls, int uniformBufferOffset, int uniformBufferElementSize)
  : vertexArrayObject(DebugMesh::generateVertexArrayObject()),
    debugMesh(std::move(debugMesh)),
    uniformBuffer(std::move(uniformBuffer)),
    shaderObject(std::move(shaderObject)),
    numDrawCalls(numDrawCalls),
    uniformBufferOffset(uniformBufferOffset),
    uniformBufferElementSize(uniformBufferElementSize)
{
}

DebugLineVisualisation::DebugLineVisualisation(DebugLineVisualisation&& other)
  : vertexArrayObject(std::move(other.vertexArrayObject)),
    debugMesh(std::move(other.debugMesh)),
    uniformBuffer(std::move(other.uniformBuffer)),
    shaderObject(std::move(other.shaderObject)),
    numDrawCalls(other.numDrawCalls),
    uniformBufferOffset(other.uniformBufferOffset),
    uniformBufferElementSize(other.uniformBufferElementSize)
{
  other.numDrawCalls = 0;
  other.uniformBufferOffset = 0;
  other.uniformBufferElementSize = 0;
}

DebugLineVisualisation::~DebugLineVisualisation()
{
}


DebugLineVisualisation::Ptr DebugLineVisualisation::drawCameras(const QVector<scene::CameraParameter>& sceneCameras)
{
  struct CachedCamera final
  {
    scene::CameraParameter cameraParameter;
    glm::mat4 inverseViewProjectionMatrix;

    CachedCamera()
    {
    }
    CachedCamera(const scene::CameraParameter& cameraParameter)
      : cameraParameter(cameraParameter)
    {
      inverseViewProjectionMatrix = glm::inverse(this->cameraParameter.projectionMatrix() * this->cameraParameter.viewMatrix());
    }
  };

  QVector<CachedCamera> cachedCameras;
  cachedCameras.reserve(sceneCameras.length());
  for(const scene::CameraParameter& cameraParameter : sceneCameras)
    cachedCameras.append(cameraParameter);

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
  painter.addCube(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));

  return Ptr(new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                 cachedCameras,
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
    uniformBuffer.BindUniformBuffer(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, i*uniformBufferOffset, uniformBufferElementSize);
    debugMesh.draw();
  }

  vertexArrayObject.ResetBinding();
}


} // namespace debugging
} // namespace glrt

