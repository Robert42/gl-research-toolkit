#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>

namespace glrt {
namespace scene {


// ======== Scene ==============================================================


Scene::Scene()
  : plainColorMeshes(gl::ShaderObject("plain-color-materials")),
    texturedMeshes(gl::ShaderObject("textured-meshes")),
    maskedMeshes(gl::ShaderObject("masked-meshes")),
    transparentMeshes(gl::ShaderObject("transparent-meshes"))
{
}

Scene::~Scene()
{
  plainColorMeshes.deinit();
  texturedMeshes.deinit();
  maskedMeshes.deinit();
  transparentMeshes.deinit();

  QSet<Entity*> entities;
  entities.swap(this->_entities);

  for(Entity* entity : entities)
    delete entity;
}


void Scene::render()
{
  // TODO bind scene uniform

  plainColorMeshes.render();
  texturedMeshes.render();
  maskedMeshes.render();
  transparentMeshes.render();
}


void Scene::AddEntity(Entity*entity)
{
  _entities.insert(entity);
}

void Scene::RemoveEntity(Entity*entity)
{
  _entities.remove(entity);
}


void Scene::AddStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  pickMaterialPass(staticMeshComponent->material.data())->AddStaticMesh(staticMeshComponent);
}

void Scene::RemoveStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  pickMaterialPass(staticMeshComponent->material.data())->RemoveStaticMesh(staticMeshComponent);
}

Scene::MaterialPass* Scene::pickMaterialPass(const Material* material)
{
  if(qobject_cast<const PlainColorMaterial*>(material) != nullptr)
      return &plainColorMeshes;
  return nullptr;
}

// ======== Scene::MaterialPass::MaterialInstance::MeshInstance ================


void Scene::MaterialPass::MaterialInstance::MeshGroup::AddStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  Q_ASSERT_X(!staticMeshComponents.contains(staticMeshComponent), __FUNCTION__, "Called AddStaticMesh, although the saticmesh component is already registered");

  staticMeshComponents.insert(staticMeshComponent, staticMeshComponent->relativeTransform);
}


void Scene::MaterialPass::MaterialInstance::MeshGroup::RemoveStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  Q_ASSERT_X(staticMeshComponents.contains(staticMeshComponent), __FUNCTION__, "Called RemoveStaticMesh, although the saticmesh component is not registered");

  staticMeshComponents.remove(staticMeshComponent);
}


bool Scene::MaterialPass::MaterialInstance::MeshGroup::isEmpty() const
{
  return staticMeshComponents.isEmpty();
}


void Scene::MaterialPass::MaterialInstance::MeshGroup::render(StaticMesh& staticMesh)
{
  // TODO: apply buffer
  staticMesh.draw();
}


// ======== Scene::MaterialPass::MaterialInstance ==============================


void Scene::MaterialPass::MaterialInstance::AddStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  StaticMesh* staticMesh = staticMeshComponent->staticMesh.data();

  staticMeshes[staticMesh].AddStaticMesh(staticMeshComponent);
}


void Scene::MaterialPass::MaterialInstance::RemoveStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  StaticMesh* staticMesh = staticMeshComponent->staticMesh.data();

  if(!staticMeshes.contains(staticMesh))
    return;

  MeshGroup& meshGroup = staticMeshes[staticMesh];

  meshGroup.RemoveStaticMesh(staticMeshComponent);

  if(meshGroup.isEmpty())
    staticMeshes.remove(staticMesh);
}


bool Scene::MaterialPass::MaterialInstance::isEmpty() const
{
  return staticMeshes.isEmpty();
}


void Scene::MaterialPass::MaterialInstance::render()
{
  for(auto i=staticMeshes.begin(); i!=staticMeshes.end(); ++i)
  {
    StaticMesh& mesh = *i.key();
    MeshGroup& meshGroup = i.value();

    meshGroup.render(mesh);
  }
}


// ======== Scene::MaterialPass ================================================


Scene::MaterialPass::MaterialPass(gl::ShaderObject&& shaderObject)
  : shaderObject(std::move(shaderObject))
{
  const std::string shaderDir = GLRT_SHADER_DIR"/materials";
  this->shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX, shaderDir + '/' + this->shaderObject.GetName() + ".vs");
  this->shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT, shaderDir + '/' + this->shaderObject.GetName() + ".fs");
}

void Scene::MaterialPass::AddStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  Material* materialInstance = staticMeshComponent->material.data();

  materialInstanceMeshList[materialInstance].AddStaticMesh(staticMeshComponent);
}

void Scene::MaterialPass::RemoveStaticMesh(StaticMeshComponent* staticMeshComponent)
{
  Material* materialInstance = staticMeshComponent->material.data();

  if(!materialInstanceMeshList.contains(materialInstance))
    return;

  MaterialInstance& materialInstanceList = materialInstanceMeshList[materialInstance];

  materialInstanceList.RemoveStaticMesh(staticMeshComponent);

  if(materialInstanceList.isEmpty())
    materialInstanceMeshList.remove(materialInstance);
}

void Scene::MaterialPass::deinit()
{
  materialInstanceMeshList.clear();
}

bool Scene::MaterialPass::isEmpty() const
{
  return materialInstanceMeshList.isEmpty();
}

void Scene::MaterialPass::render()
{
  if(isEmpty())
    return;

  shaderObject.Activate();

  for(auto i=materialInstanceMeshList.begin(); i!=materialInstanceMeshList.end(); ++i)
  {
    Material& material = *i.key();
    MaterialInstance& materialInstance = i.value();

    // TODO bind material instance uniform material->uniformBuffer

    materialInstance.render();
  }
}


} // namespace scene
} // namespace glrt

