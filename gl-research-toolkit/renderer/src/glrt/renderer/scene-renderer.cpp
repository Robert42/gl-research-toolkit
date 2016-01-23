#include <glrt/glsl/layout-constants.h>

#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/collect-scene-data.h>

#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

namespace glrt {
namespace renderer {


Renderer::Renderer(scene::Scene* scene, StaticMeshBufferManager* staticMeshBufferManager)
  : scene(*scene),
    staticMeshBufferManager(*staticMeshBufferManager),
    visualizeCameras(debugging::VisualizationRenderer::debugSceneCameras(scene)),
    visualizeSphereAreaLights(debugging::VisualizationRenderer::debugSphereAreaLights(scene)),
    visualizeRectAreaLights(debugging::VisualizationRenderer::debugRectAreaLights(scene)),
    sceneUniformBuffer(sizeof(SceneUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    staticMeshVertexArrayObject(std::move(StaticMeshBuffer::generateVertexArrayObject())),
    _directLights(new DirectLights(this))
{
}

Renderer::~Renderer()
{
  delete _directLights;
}

void Renderer::render()
{
  updateSceneUniform();

  sceneUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);

  renderImplementation();

  visualizeCameras.render();
  visualizeSphereAreaLights.render();
  visualizeRectAreaLights.render();
}

void Renderer::updateSceneUniform()
{
  SceneUniformBlock& sceneUniformData =  *reinterpret_cast<SceneUniformBlock*>(sceneUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneUniformData.view_projection_matrix = scene.debugCamera.viewProjectionMatrix;
  sceneUniformData.camera_position = scene.debugCamera.camera_position;
  sceneUniformBuffer.Unmap();
}

Renderer::DirectLights& Renderer::directLights()
{
  return *this->_directLights;
}


// ======== DirectLights =======================================================


Renderer::DirectLights::DirectLights(Renderer* renderer)
  : renderer(*renderer),
    sphereAreaShaderStorageBuffer(this->renderer.scene),
    rectAreaShaderStorageBuffer(this->renderer.scene)
{
}

Renderer::DirectLights::~DirectLights()
{
}


void Renderer::DirectLights::bindShaderStoreageBuffers(int sphereAreaLightBindingIndex, int rectAreaLightBindingIndex)
{
  sphereAreaShaderStorageBuffer.update();
  rectAreaShaderStorageBuffer.update();

  sphereAreaShaderStorageBuffer.bindShaderStorageBuffer(sphereAreaLightBindingIndex);
  rectAreaShaderStorageBuffer.bindShaderStorageBuffer(rectAreaLightBindingIndex);
}


void Renderer::DirectLights::bindShaderStoreageBuffers()
{
  bindShaderStoreageBuffers(SHADERSTORAGE_BINDING_LIGHTS_SPHEREAREA,
                            SHADERSTORAGE_BINDING_LIGHTS_RECTAREA);
}



// ======== Pass ===============================================================


Renderer::Pass::Pass(Renderer* renderer, scene::resources::Material::Type type, ReloadableShader&& shader)
  : type(type),
    renderer(*renderer),
    shader(std::move(shader)),
    materialBuffer(MaterialBuffer::Type::PLAIN_COLOR),
    isDirty(true)
{
  // #FIXME connect with signals from the scene
}

Renderer::Pass::Pass(Renderer* renderer, scene::resources::Material::Type type, const QString& materialName, const QSet<QString>& preprocessorBlock)
  : Pass(renderer,
         type,
         std::move(ReloadableShader(materialName,
                                    QDir(GLRT_SHADER_DIR"/materials"),
                                    preprocessorBlock)))
{
}

Renderer::Pass::~Pass()
{
  clearCache();
}


bool orderByDrawCall(const scene::StaticMeshComponent* a, const scene::StaticMeshComponent*b)
{
  // Movables after unmovables
  if(!a->isMovable && b->isMovable)
    return true;
  if(a->isMovable && !b->isMovable)
    return false;

  if(a->materialUuid < b->materialUuid)
    return true;
  if(a->materialUuid > b->materialUuid)
    return false;

  if(a->staticMeshUuid < b->staticMeshUuid)
    return true;
  if(a->staticMeshUuid > b->staticMeshUuid)
    return false;

  return a->uuid < b->uuid;
}


std::function<bool(scene::StaticMeshComponent* a)> allowOnly(scene::resources::Material::Type type, bool movable)
{
  return [type, movable](scene::StaticMeshComponent* a) -> bool {
    return a->isMovable==movable && a->material().type == type;
  };
}

void Renderer::Pass::render()
{
  updateCache();

  renderStaticMeshes();
}

void Renderer::Pass::renderStaticMeshes()
{
  if(materialInstanceRanges.empty())
    return;

  glEnable(GL_CULL_FACE);

  const int N = materialInstanceRanges[materialInstanceRanges.size()-1].end;

  this->shader.shaderObject.Activate();

  renderer.staticMeshVertexArrayObject.Bind();
  renderer.directLights().bindShaderStoreageBuffers();

  MaterialInstanceRange* materialInstanceRange = &materialInstanceRanges[0];
  MeshRange* meshInstanceRange = &meshRanges[0];
  gl::Buffer* buffer = staticMeshInstance_Uniforms.data();
  StaticMeshBuffer* mesh;

  mesh= meshInstanceRange->mesh;
  mesh->bind(renderer.staticMeshVertexArrayObject);
  materialBuffer.bind(0);

  for(int i=0; i<N; ++i)
  {
    if(materialInstanceRange->end == i)
    {
      ++materialInstanceRange;
      materialBuffer.bind(1);
    }
    if(meshInstanceRange->end == i)
    {
      ++meshInstanceRange;
      mesh = meshInstanceRange->mesh;

      mesh->bind(renderer.staticMeshVertexArrayObject);
    }

    buffer->BindUniformBuffer(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, i*meshInstanceUniformOffset, sizeof(MeshInstanceUniform));

    mesh->draw();
  }

  renderer.staticMeshVertexArrayObject.ResetBinding();
}

inline void Renderer::Pass::updateCache()
{
  if(!isDirty)
    return;
  isDirty = false;

  scene::Scene& scene = renderer.scene;

  aligned_vector<MeshInstanceUniform> transformations(aligned_vector<MeshInstanceUniform>::Alignment::UniformBufferOffsetAlignment);
  meshInstanceUniformOffset = transformations.alignment();

  QVector<scene::StaticMeshComponent*> allStaticMeshComponents = glrt::scene::collectAllComponentsWithType<glrt::scene::StaticMeshComponent>(&scene, allowOnly(this->type, false));

  std::sort(allStaticMeshComponents.begin(), allStaticMeshComponents.end(), orderByDrawCall);

  clearCache();

  if(!allStaticMeshComponents.isEmpty())
  {
    materialInstanceRanges.reserve(allStaticMeshComponents.length());
    meshRanges.reserve(allStaticMeshComponents.length());
    transformations.reserve(allStaticMeshComponents.length());

    materialInstanceRanges.push_back(MaterialInstanceRange{0, 1});
    meshRanges.push_back(MeshRange(MeshRange{renderer.staticMeshBufferManager.meshForUuid(allStaticMeshComponents[0]->staticMeshUuid), 0, 1}));

    MaterialInstanceRange* lastMaterialInstanceRange = &materialInstanceRanges[materialInstanceRanges.size()-1];
    MeshRange* lastMeshRange = &meshRanges[meshRanges.size()-1];
    Uuid<Material> lastMaterial = allStaticMeshComponents[0]->materialUuid;
    MaterialBuffer::Initializer materials(materialBuffer, allStaticMeshComponents.length());

    materials.append(allStaticMeshComponents[0]->material());

    for(int i=0; i<allStaticMeshComponents.size(); ++i)
    {
      scene::StaticMeshComponent* staticMeshComponent = allStaticMeshComponents[i];

      transformations.push_back(staticMeshComponent->globalCoordFrame().toMat4());

      if(staticMeshComponent->materialUuid != lastMaterial)
      {
        materials.append(staticMeshComponent->material());
        materialInstanceRanges.push_back(MaterialInstanceRange{i, i+1});
        lastMaterialInstanceRange = &materialInstanceRanges[materialInstanceRanges.size()-1];
      }

      StaticMeshBuffer* currentStaticMesh = renderer.staticMeshBufferManager.meshForUuid(staticMeshComponent->staticMeshUuid);
      if(currentStaticMesh != lastMeshRange->mesh)
      {
        meshRanges.push_back(MeshRange(MeshRange{currentStaticMesh, i, i+1}));
        lastMeshRange = &meshRanges[meshRanges.size()-1];
      }

      lastMaterialInstanceRange->end = i+1;
      lastMeshRange->end = i+1;
    }

    staticMeshInstance_Uniforms = QSharedPointer<gl::Buffer>(new gl::Buffer(transformations.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, transformations.data()));
  }
}

void Renderer::Pass::clearCache()
{
  staticMeshInstance_Uniforms.clear();
  materialInstanceRanges.clear();
  materialBuffer.clear();
  meshRanges.clear();
}


} // namespace renderer
} // namespace glrt

