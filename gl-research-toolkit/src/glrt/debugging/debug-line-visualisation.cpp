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


DebugLineVisualisation::Ptr DebugLineVisualisation::drawCameras(const QList<scene::CameraParameter>& sceneCameras)
{
  struct CachedCamera final
  {
    scene::CameraParameter cameraParameter;
    glm::mat4 inverseViewProjectionMatrix;
    glm::mat4 inverseViewMatrix;

    CachedCamera()
    {
    }
    CachedCamera(const scene::CameraParameter& cameraParameter)
      : cameraParameter(cameraParameter)
    {
      inverseViewProjectionMatrix = glm::inverse(this->cameraParameter.projectionMatrix() * this->cameraParameter.viewMatrix());
      inverseViewMatrix = this->cameraParameter.inverseViewMatrix();
    }
  };
  static_assert(sizeof(CachedCamera)==192, "Please make sure the struct CachedCamera is std140 compatible");

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



DebugLineVisualisation::Ptr DebugLineVisualisation::drawSphereAreaLights(const QList<scene::SphereAreaLightComponent::Data>& sphereAreaLights)
{
  DebugMesh::Painter painter;

  painter.addSphere(1, 16);

  return Ptr(new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                 sphereAreaLights.toVector(),
                                                                 std::move(ShaderCompiler::createShaderFromFiles("visualize-sphere-area-light",
                                                                                                                 QDir(GLRT_SHADER_DIR"/debugging/visualizations")))))));
}


DebugLineVisualisation::Ptr DebugLineVisualisation::drawRectAreaLights(const QList<scene::RectAreaLightComponent::Data>& rectAreaLights)
{
  DebugMesh::Painter painter;

  painter.addRect(glm::vec2(-1), glm::vec2(1));
  painter.addArrow(1.f, 0.1f);

  return Ptr(new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                 rectAreaLights.toVector(),
                                                                 std::move(ShaderCompiler::createShaderFromFiles("visualize-rect-area-light",
                                                                                                                 QDir(GLRT_SHADER_DIR"/debugging/visualizations")))))));
}

DebugLineVisualisation::Ptr DebugLineVisualisation::drawPositions(const QVector<glm::vec3>& positions)
{
  DebugMesh::Painter painter;

  float r = 0.1f;
  for(int i=0; i<3; ++i)
  {
    glm::vec3 a(0), b(0);

    a[i] = r;
    b[i] = -r;

    painter.addVertex(a);
    painter.addVertex(b);
  }

  return Ptr(new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                 positions,
                                                                 std::move(ShaderCompiler::createShaderFromFiles("visualize-position",
                                                                                                                 QDir(GLRT_SHADER_DIR"/debugging/visualizations")))))));
}

DebugLineVisualisation::Ptr DebugLineVisualisation::drawArrows(const QVector<Arrow>& arrows)
{
  DebugMesh::Painter painter;

  painter.addArrow(1.f, 0.1f);

  return Ptr(new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                 arrows,
                                                                 std::move(ShaderCompiler::createShaderFromFiles("visualize-arrow",
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

