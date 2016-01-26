#include <glrt/scene/camera-component.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

CameraComponent::CameraComponent(Node& node, Node::Component* parent, const Uuid<CameraComponent>& uuid, const Camera& cameraParameter, bool isMovable)
  : Component(node, parent, uuid, isMovable),
    cameraParameter(cameraParameter)
{
}


CameraComponent::~CameraComponent()
{
}


void CameraComponent::registerAngelScriptAPIDeclarations()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  glrt::Uuid<void>::registerCustomizedUuidType("CameraComponent", true);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


inline CameraComponent* createCameraComponent(Node& node,
                                              Node::Component* parent,
                                              const Uuid<CameraComponent>& uuid,
                                              bool isMovable,
                                              float aspect,
                                              float clipFar,
                                              float clipNear,
                                              float horizontal_fov,
                                              const glm::vec3& lookAt,
                                              const glm::vec3& upVector,
                                              const glm::vec3& position)
{
  Camera cameraParameter;
  cameraParameter.aspect = aspect;
  cameraParameter.clipFar = clipFar;
  cameraParameter.clipNear = clipNear;
  cameraParameter.horizontal_fov = horizontal_fov;
  cameraParameter.lookAt = lookAt;
  cameraParameter.upVector = upVector;
  cameraParameter.position = position;
  return new CameraComponent(node, parent, uuid, cameraParameter, isMovable);
}

void CameraComponent::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerCreateMethod<decltype(createCameraComponent), createCameraComponent>(angelScriptEngine,
                                                                                                "CameraComponent",
                                                                                                "const Uuid<CameraComponent> &in uuid, bool isMovable, float aspect, float clipFar, float clipNear, float horizontal_fov, const vec3 &in lookAt, const vec3 &in upVector, const vec3 &in position");

  Node::Component::registerAsBaseOfClass<CameraComponent>(angelScriptEngine, "CameraComponent");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

