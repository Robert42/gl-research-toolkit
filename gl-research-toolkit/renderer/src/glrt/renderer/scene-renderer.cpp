#include <glrt/glsl/layout-constants.h>

#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/light-component.h>
#include <glrt/scene/camera-component.h>
#include <glrt/scene/collect-scene-data.h>

#include <glrt/system.h>
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
    cameraUniformBuffer(sizeof(CameraUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    staticMeshVertexArrayObject(std::move(StaticMeshBuffer::generateVertexArrayObject())),
    _directLights(new DirectLights(this))
{
  fillCameraUniform(scene::CameraParameter());
  updateCameraUniform();
}

Renderer::~Renderer()
{
  delete _directLights;
}

void Renderer::render()
{
  updateCameraUniform();

  cameraUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);

  renderImplementation();

  visualizeCameras.render();
  visualizeSphereAreaLights.render();
  visualizeRectAreaLights.render();
}

void Renderer::updateCameraUniform()
{
  if(!this->cameraComponent)
  {
    Array<scene::CameraComponent*> cameraComponents = scene::collectAllComponentsWithType<scene::CameraComponent>(&scene);

    for(scene::CameraComponent* cameraComponent : cameraComponents)
    {
      if(cameraComponent->uuid == scene::uuids::debugCameraComponent)
      {
        this->cameraComponent = cameraComponent;
        break;
      }
    }
    if(!cameraComponent && !cameraComponents.isEmpty())
      this->cameraComponent = cameraComponents.first();
  }

  if(this->cameraComponent)
    updateCameraComponent(this->cameraComponent);
}

void Renderer::updateCameraComponent(scene::CameraComponent* cameraComponent)
{
  Q_ASSERT(cameraComponent != nullptr);
  cameraComponent->cameraParameter.aspect = System::windowAspectRatio();
  fillCameraUniform(cameraComponent->globalCameraParameter());
}

void Renderer::fillCameraUniform(const scene::CameraParameter& cameraParameter)
{
  CameraUniformBlock& cameraUniformData =  *reinterpret_cast<CameraUniformBlock*>(cameraUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  cameraUniformData.view_projection_matrix = cameraParameter.projectionMatrix() * cameraParameter.viewMatrix();
  cameraUniformData.camera_position = cameraParameter.position;
  cameraUniformBuffer.Unmap();
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
  // #TODO not pretty, we are just interested in changes of static mesh components
  connect(&renderer->scene, SIGNAL(sceneCleared()), this, SLOT(markDirty()));
  connect(&renderer->scene, SIGNAL(sceneLoaded(bool)), this, SLOT(markDirty()));
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
  if(!a->movable() && b->movable())
    return true;
  if(a->movable() && !b->movable())
    return false;

  if(a->material().materialUser < b->material().materialUser)
    return true;
  if(a->material().materialUser > b->material().materialUser)
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


std::function<bool(scene::StaticMeshComponent* a)> allowOnly(scene::resources::Material::Type type)
{
  return [type](scene::StaticMeshComponent* a) -> bool {
    return a->material().type == type;
  };
}

void Renderer::Pass::render()
{
  updateCache();

  renderStaticMeshes();
}

void Renderer::Pass::markDirty()
{
  isDirty = true;
}

void Renderer::Pass::renderStaticMeshes()
{
  if(materialRanges.empty())
    return;

  glEnable(GL_CULL_FACE);

  const int N = materialRanges[materialRanges.size()-1].end;

  this->shader.shaderObject.Activate();

  renderer.staticMeshVertexArrayObject.Bind();
  renderer.directLights().bindShaderStoreageBuffers();

  MaterialRange* materialInstanceRange = &materialRanges[0];
  MeshRange* meshInstanceRange = &meshRanges[0];
  gl::Buffer* buffer = staticMeshInstance_Uniforms.data();
  StaticMeshBuffer* mesh;

  mesh= meshInstanceRange->mesh;
  mesh->bind(renderer.staticMeshVertexArrayObject);

  int material = 0;
  materialBuffer.bind(material);

  for(int i=0; i<N; ++i)
  {
    if(materialInstanceRange->end == i)
    {
      ++materialInstanceRange;
      ++material;
      materialBuffer.bind(material);
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

struct Renderer::Pass::StaticMeshBufferVerification
{
  QHash<int, Uuid<Material>> materials;
  QHash<int, Uuid<StaticMesh>> meshes;

  struct Instance
  {
    int material;
    int mesh;
  };

  QVector<Instance> instances;

  void appendMaterial(const Uuid<Material>& material)
  {
    materials[materials.size()] = material;
  }

  void appendStaticMesh(const Uuid<StaticMesh>& mesh)
  {
    meshes[meshes.size()] = mesh;
  }

  void addInstance()
  {
    instances.push_back(Instance{materials.size()-1, meshes.size()-1});
  }

  void verify(const Array<scene::StaticMeshComponent*>& comparison,
              const QVector<MaterialRange>& materialRanges,
              const QVector<MeshRange>& meshRanges) const
  {
    Q_ASSERT(comparison.length() == instances.length());
    Q_ASSERT(materialRanges.length() == materials.size());
    Q_ASSERT(meshRanges.length() == meshes.size());

    for(int i=0; i<comparison.length(); ++i)
    {
      Q_ASSERT(comparison[i]->materialUuid == materials[instances[i].material]);
      Q_ASSERT(comparison[i]->staticMeshUuid == meshes[instances[i].mesh]);
    }

    for(int i=0; i<materialRanges.size(); ++i)
      for(int j=materialRanges[i].begin; j<materialRanges[i].end; ++j)
        Q_ASSERT(instances[j].material == i);

    for(int i=0; i<meshRanges.size(); ++i)
      for(int j=meshRanges[i].begin; j<meshRanges[i].end; ++j)
        Q_ASSERT(instances[j].mesh == i);
  }
};

inline void Renderer::Pass::updateCache()
{
  if(!isDirty)
    return;
  isDirty = false;

  StaticMeshBufferVerification verification;

  scene::Scene& scene = renderer.scene;

  aligned_vector<MeshInstanceUniform> transformations(aligned_vector<MeshInstanceUniform>::Alignment::UniformBufferOffsetAlignment);
  meshInstanceUniformOffset = transformations.alignment();

  Array<scene::StaticMeshComponent*> allStaticMeshComponents = glrt::scene::collectAllComponentsWithType<glrt::scene::StaticMeshComponent>(&scene, allowOnly(this->type));

  std::sort(allStaticMeshComponents.begin(), allStaticMeshComponents.end(), orderByDrawCall);

  clearCache();

  if(!allStaticMeshComponents.isEmpty())
  {
    materialRanges.reserve(allStaticMeshComponents.length());
    meshRanges.reserve(allStaticMeshComponents.length());
    transformations.reserve(allStaticMeshComponents.length());

    MeshRange* lastMeshRange = nullptr;
    MaterialRange* lastMaterialInstanceRange = nullptr;
    Uuid<StaticMesh> lastMesh;
    Uuid<Material> lastMaterial;
    MaterialBuffer::Initializer materials(materialBuffer, allStaticMeshComponents.length());

    for(int i=0; i<allStaticMeshComponents.length(); ++i)
    {
      scene::StaticMeshComponent* staticMeshComponent = allStaticMeshComponents[i];

      transformations.push_back(staticMeshComponent->globalCoordFrame().toMat4());

      if(staticMeshComponent->materialUuid != lastMaterial)
      {
        materials.append(staticMeshComponent->material());
        verification.appendMaterial(staticMeshComponent->materialUuid);
        materialRanges.push_back(MaterialRange{i, i+1});
        lastMaterialInstanceRange = &materialRanges[materialRanges.size()-1];
      }

      StaticMeshBuffer* currentStaticMesh = renderer.staticMeshBufferManager.meshForUuid(staticMeshComponent->staticMeshUuid);
      if(staticMeshComponent->staticMeshUuid != lastMesh)
      {
        verification.appendStaticMesh(staticMeshComponent->staticMeshUuid);
        meshRanges.push_back(MeshRange(MeshRange{currentStaticMesh, i, i+1}));
        lastMeshRange = &meshRanges[meshRanges.size()-1];
      }

      Q_ASSERT(lastMeshRange!=nullptr);
      Q_ASSERT(lastMaterialInstanceRange!=nullptr);

      verification.addInstance();
      lastMaterialInstanceRange->end = i+1;
      lastMeshRange->end = i+1;
    }

    verification.verify(allStaticMeshComponents, materialRanges, meshRanges);

    staticMeshInstance_Uniforms = QSharedPointer<gl::Buffer>(new gl::Buffer(transformations.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, transformations.data()));
  }
}

void Renderer::Pass::clearCache()
{
  staticMeshInstance_Uniforms.clear();
  materialRanges.clear();
  materialBuffer.clear();
  meshRanges.clear();
}


} // namespace renderer
} // namespace glrt

