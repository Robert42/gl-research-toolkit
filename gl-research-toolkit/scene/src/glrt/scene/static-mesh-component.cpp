#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/resources/material.h>
#include <glrt/scene/resources/resource-manager.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

StaticMeshComponent::StaticMeshComponent(Node& entity, // #TODO rename all entity/entities to node
                                         const Uuid<StaticMeshComponent>& uuid,
                                         bool isMovable,
                                         const Uuid<resources::StaticMeshData>& staticMesh,
                                         const Uuid<resources::Material>& materialUuid)
  : Node::Component(entity, uuid, isMovable),
    staticMeshUuid(staticMesh),
    materialUuid(materialUuid)
{
}

StaticMeshComponent::~StaticMeshComponent()
{
}


void StaticMeshComponent::registerAngelScriptAPIDeclarations()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  glrt::Uuid<void>::registerCustomizedUuidType("StaticMeshComponent", true);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

inline StaticMeshComponent* createStaticMeshComponent(Node* node,
                                                      const Uuid<StaticMeshComponent>& uuid,
                                                      bool isMovable,
                                                      const Uuid<resources::StaticMeshData>& meshUuid,
                                                      const Uuid<resources::Material>& materialUuid)
{
  return new StaticMeshComponent(*node, uuid, isMovable, meshUuid, materialUuid);
}

void StaticMeshComponent::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectMethod("Node", "StaticMeshComponent@ newStaticMeshComponent(const Uuid<StaticMeshComponent> &in uuid, bool isMovable, const Uuid<StaticMesh> &in meshUuid, const Uuid<Material> &in materialUuid)", AngelScript::asFUNCTION(createStaticMeshComponent), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  Node::Component::registerAsBaseOfClass<StaticMeshComponent>(angelScriptEngine, "StaticMeshComponent");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

resources::Material StaticMeshComponent::material() const
{
  return this->node.resourceManager().materialSourceForUuid(this->materialUuid);
}


} // namespace scene
} // namespace glrt

