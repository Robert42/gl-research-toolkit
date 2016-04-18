#include <glrt/glsl/layout-constants.h>
#include <glrt/scene/scene.h>
#include <glrt/renderer/debugging/debug-line-visualisation.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

namespace glrt {
namespace renderer {
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


DebugRenderer::Implementation* DebugLineVisualisation::drawCameras(const QList<scene::CameraParameter>& sceneCameras)
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

  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             cachedCameras,
                                                             std::move(ShaderCompiler::createShaderFromFiles("visualize-scene-camera",
                                                                                                             QDir(GLRT_SHADER_DIR"/debugging/visualizations"))))));
}



DebugRenderer::Implementation* DebugLineVisualisation::drawSphereAreaLights(const QList<scene::SphereAreaLightComponent::Data>& sphereAreaLights)
{
  DebugMesh::Painter painter;

  painter.addSphere(1, 16);

  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             sphereAreaLights.toVector(),
                                                             std::move(ShaderCompiler::createShaderFromFiles("visualize-sphere-area-light",
                                                                                                             QDir(GLRT_SHADER_DIR"/debugging/visualizations"))))));
}


DebugRenderer::Implementation* DebugLineVisualisation::drawRectAreaLights(const QList<scene::RectAreaLightComponent::Data>& rectAreaLights)
{
  DebugMesh::Painter painter;

  painter.addRect(glm::vec2(-1), glm::vec2(1));
  painter.addArrow(1.f, 0.1f);

  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             rectAreaLights.toVector(),
                                                             std::move(ShaderCompiler::createShaderFromFiles("visualize-rect-area-light",
                                                                                                             QDir(GLRT_SHADER_DIR"/debugging/visualizations"))))));
}

DebugRenderer::Implementation* DebugLineVisualisation::drawPositions(const QVector<glm::vec3>& positions)
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

  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             positions,
                                                             std::move(ShaderCompiler::createShaderFromFiles("visualize-position",
                                                                                                             QDir(GLRT_SHADER_DIR"/debugging/visualizations"))))));
}

DebugRenderer::Implementation* DebugLineVisualisation::drawArrows(const QVector<Arrow>& arrows)
{
  DebugMesh::Painter painter;

  painter.addArrow(1.f, 0.1f);

  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             arrows,
                                                             std::move(ShaderCompiler::createShaderFromFiles("visualize-arrow",
                                                                                                             QDir(GLRT_SHADER_DIR"/debugging/visualizations"))))));
}


DebugRenderer::Implementation* DebugLineVisualisation::drawVoxelGrids(const QList<scene::VoxelDataComponent::Data>& voxelData)
{
  DebugMesh::Painter painter;

  glm::ivec3 maxGridSize(0);

  for(const scene::VoxelDataComponent::Data& data : voxelData)
    maxGridSize = glm::max(maxGridSize, data.voxelCount);

  for(int dimension = 0; dimension<3; ++dimension)
  {
    const int n = maxGridSize[dimension];
    glm::mat4 matrix(1);
    std::swap(matrix[2], matrix[dimension]);
    painter.pushMatrix(matrix);
    for(int i=0; i<n; ++i)
    {
      painter.pushMatrix(glm::vec3(0, 0, i));
      painter.addRect(glm::vec2(0), glm::vec2(1));
      painter.popMatrix();
    }
    painter.popMatrix();
  }

  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             voxelData.toVector(),
                                                             std::move(ShaderCompiler::createShaderFromFiles("visualize-voxel-grids",
                                                                                                             QDir(GLRT_SHADER_DIR"/debugging/visualizations"))))));
}


void DebugLineVisualisation::render()
{
  bool use_depth_test = glIsEnabled(GL_DEPTH_TEST);

  if(use_depth_test)
    glDisable(GL_DEPTH_TEST);

  shaderObject.Activate();

  vertexArrayObject.Bind();
  debugMesh.bind(this->vertexArrayObject);

  for(int i=0; i<numDrawCalls; ++i)
  {
    uniformBuffer.BindUniformBuffer(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, i*uniformBufferOffset, uniformBufferElementSize);
    debugMesh.draw();
  }

  vertexArrayObject.ResetBinding();

  if(use_depth_test)
    glEnable(GL_DEPTH_TEST);
}


} // namespace debugging
} // namespace renderer
} // namespace glrt

