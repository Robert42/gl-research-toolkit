#include <glrt/glsl/layout-constants.h>
#include <glrt/scene/scene.h>
#include <glrt/renderer/debugging/debug-line-visualisation.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {
namespace debugging {


inline QStringList proprocessorBlock()
{
  return (ReloadableShader::globalPreprocessorBlock | QSet<QString>()).toList();
}


DebugLineVisualisation::DebugLineVisualisation(DebugMesh&& debugMesh, gl::Buffer&& uniformBuffer, gl::Program&& glProgram, int numDrawCalls, int uniformBufferOffset, int uniformBufferElementSize)
  : vertexArrayObject(DebugMesh::generateVertexArrayObject()),
    debugMesh(std::move(debugMesh)),
    uniformBuffer(std::move(uniformBuffer)),
    glProgram(std::move(glProgram)),
    numDrawCalls(numDrawCalls),
    uniformBufferOffset(uniformBufferOffset),
    uniformBufferElementSize(uniformBufferElementSize)
{
}

DebugLineVisualisation::DebugLineVisualisation(DebugLineVisualisation&& other)
  : vertexArrayObject(std::move(other.vertexArrayObject)),
    debugMesh(std::move(other.debugMesh)),
    uniformBuffer(std::move(other.uniformBuffer)),
    glProgram(std::move(other.glProgram)),
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

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             cachedCameras,
                                                             std::move(shaderCompiler.compileProgramFromFiles("visualize-scene-camera",
                                                                                                              QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                              proprocessorBlock())))));
}



DebugRenderer::Implementation* DebugLineVisualisation::drawSphereAreaLights(const QList<scene::SphereAreaLightComponent::Data>& sphereAreaLights)
{
  DebugMesh::Painter painter;

  painter.addSphere(1, 16);

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             sphereAreaLights.toVector(),
                                                             std::move(shaderCompiler.compileProgramFromFiles("visualize-sphere-area-light",
                                                                                                              QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                              proprocessorBlock())))));
}


DebugRenderer::Implementation* DebugLineVisualisation::drawRectAreaLights(const QList<scene::RectAreaLightComponent::Data>& rectAreaLights)
{
  DebugMesh::Painter painter;

  painter.addRect(glm::vec2(-1), glm::vec2(1));
  painter.addArrow(1.f, 0.1f);

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             rectAreaLights.toVector(),
                                                             std::move(shaderCompiler.compileProgramFromFiles("visualize-rect-area-light",
                                                                                                              QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                              proprocessorBlock())))));
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

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             positions,
                                                             std::move(shaderCompiler.compileProgramFromFiles("visualize-position",
                                                                                                              QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                              proprocessorBlock())))));
}

DebugRenderer::Implementation* DebugLineVisualisation::drawArrows(const QVector<Arrow>& arrows)
{
  DebugMesh::Painter painter;

  painter.addArrow(1.f, 0.1f);

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             arrows,
                                                             std::move(shaderCompiler.compileProgramFromFiles("visualize-arrow",
                                                                                                              QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                              proprocessorBlock())))));
}

DebugRenderer::Implementation* DebugLineVisualisation::drawCones(const QVector<Cone>& cones)
{
  DebugMesh::Painter painter;

  painter.pushMatrix(glm::vec3(0, 0, 1));
  painter.addCircle(1.f, 96);
  painter.popMatrix();
  painter.addVertex(-1, 0, 1);
  painter.addVertex( 0, 0, 0);
  painter.addVertex( 1, 0, 1);
  painter.addVertex( 0, 0, 0);
  painter.addVertex( 0,-1, 1);
  painter.addVertex( 0, 0, 0);
  painter.addVertex( 0, 1, 1);
  painter.addVertex( 0, 0, 0);

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  return new DebugLineVisualisation(std::move(debugRendering(painter,
                                                             cones,
                                                             std::move(shaderCompiler.compileProgramFromFiles("visualize-cone",
                                                                                                              QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                              proprocessorBlock())))));
}


DebugRenderer::Implementation* DebugLineVisualisation::drawWorldGrid()
{
  DebugMesh::Painter painter;

  // Tango Desktop theme: https://en.wikipedia.org/w/index.php?title=Tango_Desktop_Project&oldid=705500829
  glm::vec3 grey = vec3FromRgb(0x888a85);
  glm::vec3 red = vec3FromRgb(0xef2929);
  glm::vec3 green = vec3FromRgb(0x8ae234);
  glm::vec3 blue = vec3FromRgb(0x729fcf);

  QVector<int> vector = {42};

  painter.nextAttribute.color = grey;
  int l = 1;
  for(int x=-l; x<=l; ++x)
  {
    for(int y=-l; y<=l; ++y)
    {
      int X = x+1;
      int Y = y+1;
      painter.addVertex( x, Y);
      painter.addVertex( x,-Y);
      painter.addVertex( X, y);
      painter.addVertex(-X, y);
    }
  }

  painter.nextAttribute.color = red;
  painter.pushMatrix(glm::vec3(0), glm::vec3(1,0,0));
  painter.addArrow(1.f, 0.1f);
  painter.popMatrix();

  painter.nextAttribute.color = green;
  painter.pushMatrix(glm::vec3(0), glm::vec3(0,1,0));
  painter.addArrow(1.f, 0.1f);
  painter.popMatrix();

  painter.nextAttribute.color = blue;
  painter.pushMatrix(glm::vec3(0), glm::vec3(0,0,1));
  painter.addArrow(1.f, 0.1f);
  painter.popMatrix();

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  vector,
                                                                                  std::move(shaderCompiler.compileProgramFromFiles("visualize-position",
                                                                                                                                   QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                   proprocessorBlock())))));
  return v;
}

DebugRenderer::Implementation*DebugLineVisualisation::drawUniformTest()
{
  QVector<int> vector = {42};
  DebugMesh::Painter painter;

  for(int i=0; i<8; ++i)
  {
    glm::vec3 offset(i&1, (i&2)>>1, (i&4)>>2);
    painter.addCube(offset+glm::vec3(-1),
                    offset+glm::vec3(0));
  }

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  vector,
                                                                                  std::move(shaderCompiler.compileProgramFromFiles_WithDebugger("visualize-uniform-test",
                                                                                                                                                QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                                proprocessorBlock())))));
  return v;
}


DebugRenderer::Implementation* DebugLineVisualisation::drawVoxelGrids(const QList<VoxelBoundingBox>& gridSizes)
{
  DebugMesh::Painter painter;

  glm::ivec3 maxGridSize(0);

  for(const VoxelBoundingBox& bb : gridSizes)
    maxGridSize = glm::max(maxGridSize, bb.voxelCount);

  for(int dimension = 0; dimension<3; ++dimension)
  {
    glm::vec3 color(0);
    color[dimension] = 1;

    const int n = maxGridSize[dimension];
    glm::mat4 matrix = glm::mat4(1);
    std::swap(matrix[2], matrix[dimension]);
    painter.pushMatrix(matrix);
    for(int i=0; i<=n; ++i)
    {
      glm::vec3 color(0);
      color[dimension] = i;

      painter.nextAttribute.color = color;
      painter.addRect(glm::vec2(0), glm::vec2(1));
    }
    painter.popMatrix();
  }

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  gridSizes.toVector(),
                                                                                  std::move(shaderCompiler.compileProgramFromFiles("visualize-voxel-grids",
                                                                                                                                    QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                   proprocessorBlock())))));
  v->use_dephtest = true;
  return v;
}

DebugRenderer::Implementation* DebugLineVisualisation::drawSpheres(const QList<BoundingSphere>& spheres)
{
  DebugMesh::Painter painter;

  painter.addSphere(1.f, 96);

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  spheres.toVector(),
                                                                                  std::move(shaderCompiler.compileProgramFromFiles("visualize-spheres",
                                                                                                                                   QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                   proprocessorBlock())))));
  v->use_dephtest = true;
  return v;
}


DebugRenderer::Implementation* DebugLineVisualisation::drawBoundingBoxes(const QVector<scene::AABB>& boundingBoxes)
{
  DebugMesh::Painter painter;

  painter.addCube(glm::vec3(0), glm::vec3(1));

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  boundingBoxes,
                                                                                  std::move(shaderCompiler.compileProgramFromFiles("visualize-aabbs",
                                                                                                                                   QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                   proprocessorBlock())))));
  v->use_dephtest = true;
  return v;
}

DebugRenderer::Implementation*DebugLineVisualisation::drawBvh(quint16 num_bvh_leaves)
{
  DebugMesh::Painter painter;

  painter.nextAttribute.color = glm::vec3(1, 0, 0);
  painter.addSphere(1.f, 96);
  painter.nextAttribute.color = glm::vec3(0, 1, 0);
  painter.addVertex(0, 0, 0);
  painter.addVertex(1, 0, 0);
  painter.nextAttribute.color = glm::vec3(0, 0, 1);
  painter.addVertex(0, 0, 0);
  painter.addVertex(1, 0, 0);

  QVector<uint16_t> nodes;
  nodes.resize(num_bvh_leaves-1);
  for(uint16_t i=0; i<uint16_t(nodes.length()); ++i)
  {
    nodes[i] = i;
  }

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  nodes,
                                                                                  std::move(shaderCompiler.compileProgramFromFiles("visualize-bvh",
                                                                                                                                   QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                   proprocessorBlock())))));
  v->use_dephtest = true;
  return v;
}

DebugRenderer::Implementation*DebugLineVisualisation::drawBvh_Grid(quint16 num_bvh_grids)
{
  DebugMesh::Painter painter;

  int gridSize = 16;

  for(int dimension = 0; dimension<3; ++dimension)
  {
    const int n = gridSize;
    glm::mat4 matrix = glm::mat4(1);
    std::swap(matrix[2], matrix[dimension]);
    painter.pushMatrix(matrix);
    for(int i=0; i<=n; ++i)
    {
      painter.pushMatrix(glm::vec3(0, 0, i));
      painter.addRect(glm::vec2(0), glm::vec2(gridSize));
      painter.popMatrix();
    }
    painter.popMatrix();
  }


  QVector<uint16_t> grids;
  grids.resize(num_bvh_grids);
  for(quint16 i=0; i<num_bvh_grids; ++i)
  {
    grids[i] = num_bvh_grids-1-i;
  }

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  DebugLineVisualisation* v = new DebugLineVisualisation(std::move(debugRendering(painter,
                                                                                  grids,
                                                                                  std::move(shaderCompiler.compileProgramFromFiles("visualize-bvh-cascaded-grids",
                                                                                                                                   QDir(GLRT_SHADER_DIR"/debugging/visualizations"),
                                                                                                                                   proprocessorBlock())))));
  v->use_dephtest = true;
  return v;
}


void DebugLineVisualisation::render()
{
  bool temporary_disable_depthtest = !use_dephtest && glIsEnabled(GL_DEPTH_TEST);

  if(temporary_disable_depthtest)
    glDisable(GL_DEPTH_TEST);

  glProgram.use();

  vertexArrayObject.Bind();
  debugMesh.bind(this->vertexArrayObject);

  for(int i=0; i<numDrawCalls; ++i)
  {
    uniformBuffer.BindUniformBuffer(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, i*uniformBufferOffset, uniformBufferElementSize);
    debugMesh.draw();
  }

  gl::Program::useNone();

  vertexArrayObject.ResetBinding();

  if(temporary_disable_depthtest)
    glEnable(GL_DEPTH_TEST);
}


} // namespace debugging
} // namespace renderer
} // namespace glrt

