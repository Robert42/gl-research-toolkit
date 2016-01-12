#include <glrt/scene/static-mesh-component.h>
#include <glrt/scene/scene.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

StaticMeshComponent::StaticMeshComponent(Node& entity,
                                         const Uuid<StaticMeshComponent>& uuid,
                                         bool isMovable,
                                         const Uuid<resources::StaticMeshData>& staticMesh,
                                         const Uuid<resources::Material>& material)
  : Node::Component(entity, uuid, isMovable),
    staticMesh(staticMesh),
    material(material)
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


inline StaticMeshComponent* createStaticMeshComponent(const Uuid<StaticMeshComponent>& uuid,
                                                      Node* node,
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

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

