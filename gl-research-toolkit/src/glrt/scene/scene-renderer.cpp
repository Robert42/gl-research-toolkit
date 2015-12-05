#include <glrt/scene/scene-renderer.h>
#include <glrt/scene/static-mesh-component.h>

#include <glrt/glsl/layout-constants.h>
#include <glrt/toolkit/aligned-vector.h>
#include <glrt/toolkit/shader-compiler.h>

namespace glrt {
namespace scene {


Renderer::Renderer(Scene* scene)
  : scene(*scene),
    sceneUniformBuffer(sizeof(SceneUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    staticMeshVertexArrayObject(std::move(StaticMesh::generateVertexArrayObject()))
{
}

Renderer::~Renderer()
{
}


void Renderer::render()
{
  updateSceneUniform();

  sceneUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);

  renderImplementation();
}

void Renderer::updateSceneUniform()
{
  SceneUniformBlock& sceneUniformData =  *reinterpret_cast<SceneUniformBlock*>(sceneUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneUniformData.view_projection_matrix = scene.debugCamera.viewProjectionMatrix;
  sceneUniformBuffer.Unmap();
}


// ======== Pass ===============================================================


Renderer::Pass::Pass(Renderer* renderer, MaterialInstance::Type type, gl::ShaderObject&& shaderObject)
  : type(type),
    renderer(*renderer),
    shaderObject(std::move(shaderObject))
{
}

Renderer::Pass::Pass(Renderer* renderer, MaterialInstance::Type type, const QString& materialName, const QStringList& preprocessorBlock)
  : Pass(renderer, type, std::move(gl::ShaderObject(materialName.toStdString())))
{
  ShaderCompiler compiler;

  compiler.preprocessorBlock = preprocessorBlock;

  const QDir shaderDir(GLRT_SHADER_DIR"/materials");

  compiler.compile(&this->shaderObject, shaderDir);
}

Renderer::Pass::~Pass()
{
  clearCache();
}


bool orderByDrawCall(StaticMeshComponent* a, StaticMeshComponent*b)
{
  // Movables after unmovables
  if(!a->movable)
    return true;
  if(a->movable)
    return false;

  if(a->materialInstance.data() < b->materialInstance.data())
    return true;
  if(a->materialInstance.data() > b->materialInstance.data())
    return false;

  if(a->staticMesh.data() < b->staticMesh.data())
    return true;
  if(a->staticMesh.data() > b->staticMesh.data())
    return false;

  return a < b;
}


std::function<bool(StaticMeshComponent* a)> allowOnly(MaterialInstance::Type type, bool movable)
{
  return [movable, type](StaticMeshComponent* a) {
    return a->movable==movable && a->materialInstance->type == type;
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

  this->shaderObject.Activate();

  renderer.staticMeshVertexArrayObject.Bind();

  MaterialInstanceRange* materialInstanceRange = &materialInstanceRanges[0];
  MeshRange* meshInstanceRange = &meshRanges[0];
  gl::Buffer* buffer = staticMeshInstance_Uniforms.data();
  StaticMesh* mesh;

  mesh= meshInstanceRange->mesh;
  mesh->bind(renderer.staticMeshVertexArrayObject);
  materialInstanceRange->materialInstance->uniformBuffer.BindUniformBuffer(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK);

  for(int i=0; i<N; ++i)
  {
    if(materialInstanceRange->end == i)
    {
      ++materialInstanceRange;
      materialInstanceRange->materialInstance->uniformBuffer.BindUniformBuffer(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK);
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
  Scene& scene = renderer.scene;

  if(_cachedStaticStructureCacheIndex == scene._cachedStaticStructureCacheIndex)
    return;

  aligned_vector<MeshInstanceUniform> transformations(aligned_vector<MeshInstanceUniform>::Alignment::UniformBufferOffsetAlignment);
  meshInstanceUniformOffset = transformations.alignment();

  QVector<StaticMeshComponent*> allStaticMeshComponents = scene.allComponentsWithType<StaticMeshComponent>(allowOnly(this->type, false));

  qSort(allStaticMeshComponents.begin(), allStaticMeshComponents.end(), orderByDrawCall);

  clearCache();

  if(!allStaticMeshComponents.isEmpty())
  {
    materialInstanceRanges.reserve(allStaticMeshComponents.size());
    meshRanges.reserve(allStaticMeshComponents.size());
    transformations.reserve(allStaticMeshComponents.size());

    materialInstanceRanges.push_back(MaterialInstanceRange{allStaticMeshComponents[0]->materialInstance.data(), 0, 1});
    meshRanges.push_back(MeshRange(MeshRange{allStaticMeshComponents[0]->staticMesh.data(), 0, 1}));

    MaterialInstanceRange* lastMaterialInstanceRange = &materialInstanceRanges[materialInstanceRanges.size()-1];
    MeshRange* lastMeshRange = &meshRanges[meshRanges.size()-1];

    for(int i=0; i<allStaticMeshComponents.size(); ++i)
    {
      StaticMeshComponent* staticMeshComponent = allStaticMeshComponents[i];

      transformations.push_back(staticMeshComponent->globalTransformation());

      if(staticMeshComponent->materialInstance.data() != lastMaterialInstanceRange->materialInstance)
      {
        materialInstanceRanges.push_back(MaterialInstanceRange{staticMeshComponent->materialInstance.data(), i, i+1});
        lastMaterialInstanceRange = &materialInstanceRanges[materialInstanceRanges.size()-1];
      }

      if(staticMeshComponent->staticMesh.data() != lastMeshRange->mesh)
      {
        meshRanges.push_back(MeshRange(MeshRange{staticMeshComponent->staticMesh.data(), i, i+1}));
        lastMeshRange = &meshRanges[meshRanges.size()-1];
      }

      lastMaterialInstanceRange->end = i+1;
      lastMeshRange->end = i+1;
    }

    staticMeshInstance_Uniforms = QSharedPointer<gl::Buffer>(new gl::Buffer(transformations.size_in_bytes(), gl::Buffer::UsageFlag::IMMUTABLE, transformations.data()));
  }

  _cachedStaticStructureCacheIndex = scene._cachedStaticStructureCacheIndex;
}

void Renderer::Pass::clearCache()
{
  staticMeshInstance_Uniforms.clear();
  materialInstanceRanges.clear();
  meshRanges.clear();
}


} // namespace scene
} // namespace glrt

