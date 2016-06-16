#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>
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
  : Node::Component(node, parent, uuid),
    staticMeshUuid(staticMesh),
    materialUuid(materialUuid)
{
  resourceManager().addMaterialUser(materialUuid, staticMesh);
  resourceManager().loadStaticMesh(staticMesh);
  scene().aabb |= resourceManager().staticMeshAABB(staticMesh);
  scene().StaticMeshComponentAdded(this);
}

StaticMeshComponent::~StaticMeshComponent()
{
  hideInDestructor();
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
  return this->node.resourceManager().materialForUuid(this->materialUuid);
}


} // namespace scene
} // namespace glrt

