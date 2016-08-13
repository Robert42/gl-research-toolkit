#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/resources/resource-manager.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

StaticMeshComponent::StaticMeshComponent(Node& node,
                                         Node::Component* parent,
                                         const Uuid<StaticMeshComponent>& uuid,
                                         const Uuid<resources::StaticMesh>& staticMesh,
                                         const Uuid<resources::Material>& materialUuid)
  : ComponentWithAABB(node, parent, uuid, DataClass::STATICMESH)
{
  resourceManager().addMaterialUser(materialUuid, staticMesh);
  resourceManager().loadStaticMesh(staticMesh);
  localAabb = resourceManager().staticMeshAABB(staticMesh);

  const quint16 index = data_index.array_index;
  Scene::Data::StaticMeshes* staticMeshes = scene().data->staticMeshes;
  staticMeshes->materialUuid[index] = materialUuid;
  staticMeshes->staticMeshUuid[index] = staticMesh;

  scene().StaticMeshComponentAdded(this);
}

StaticMeshComponent::~StaticMeshComponent()
{
  hideInDestructor();

  Scene::Data::StaticMeshes* staticMeshes = scene().data->staticMeshes;
  staticMeshes->swap_staticmesh_data(data_index.array_index, staticMeshes->last_item_index());
}

const Uuid<resources::StaticMesh>& StaticMeshComponent::staticMeshUuid() const
{
  const quint16 index = data_index.array_index;
  const Scene::Data::StaticMeshes* staticMeshes = scene().data->staticMeshes;

  return staticMeshes->staticMeshUuid[index];
}

const Uuid<resources::Material>& StaticMeshComponent::materialUuid() const
{
  const quint16 index = data_index.array_index;
  const Scene::Data::StaticMeshes* staticMeshes = scene().data->staticMeshes;

  return staticMeshes->materialUuid[index];
}


void StaticMeshComponent::registerAngelScriptAPIDeclarations()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  glrt::Uuid<void>::registerCustomizedUuidType("StaticMeshComponent", true);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

inline StaticMeshComponent* createStaticMeshComponent(Node& node,
                                                      Node::Component* parent,
                                                      const Uuid<StaticMeshComponent>& uuid,
                                                      const Uuid<resources::StaticMesh>& meshUuid,
                                                      const Uuid<resources::Material>& materialUuid)
{
  return new StaticMeshComponent(node, parent, uuid, meshUuid, materialUuid);
}

void StaticMeshComponent::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerCreateMethod<decltype(createStaticMeshComponent), createStaticMeshComponent>(angelScriptEngine,
                                                                                                        "StaticMeshComponent",
                                                                                                        "const Uuid<StaticMeshComponent> &in uuid, const Uuid<StaticMesh> &in meshUuid, const Uuid<Material> &in materialUuid");

  Node::Component::registerAsBaseOfClass<StaticMeshComponent>(angelScriptEngine, "StaticMeshComponent");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

resources::Material StaticMeshComponent::material() const
{
  return this->node.resourceManager().materialForUuid(materialUuid());
}


} // namespace scene
} // namespace glrt

