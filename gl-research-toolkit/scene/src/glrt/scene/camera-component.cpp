#include <glrt/scene/camera-component.h>
#include <glrt/scene/scene-layer.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

CameraComponent::CameraComponent(Node& node, Node::Component* parent, const Uuid<CameraComponent>& uuid, const CameraParameter& cameraParameter)
  : Component(node, parent, uuid, DataClass::CAMERA | DataClass::DYNAMIC),
    cameraParameter(cameraParameter)
{
  Q_ASSERT(isDynamic());
  scene().CameraComponentAdded(this);
}


CameraComponent::~CameraComponent()
{
  hideInDestructor();

  Scene::Data::Cameras& cameras = scene().data->cameras;
  cameras.swap_camera_data(data_index.array_index, cameras.last_item_index());
}


CameraParameter CameraComponent::globalCameraParameter() const
{
  return this->globalCoordFrame() * this->cameraParameter;
}

void CameraComponent::set_localCoordFrame(const CoordFrame& coordFrame)
{
  Q_ASSERT(isDynamic());
  Node::Component::set_localCoordFrame(coordFrame);
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
                                              float aspect,
                                              float clipFar,
                                              float clipNear,
                                              float horizontal_fov,
                                              const glm::vec3& lookAt,
                                              const glm::vec3& upVector,
                                              const glm::vec3& position)
{
  CameraParameter cameraParameter;
  cameraParameter.aspect = aspect;
  cameraParameter.clipFar = clipFar;
  cameraParameter.clipNear = clipNear;
  cameraParameter.horizontal_fov = horizontal_fov;
  cameraParameter.lookAt = lookAt;
  cameraParameter.upVector = upVector;
  cameraParameter.position = position;
  return new CameraComponent(node, parent, uuid, cameraParameter);
}

void CameraComponent::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  Node::Component::registerCreateMethod<decltype(createCameraComponent), createCameraComponent>(angelScriptEngine,
                                                                                                "CameraComponent",
                                                                                                "const Uuid<CameraComponent> &in uuid, float aspect, float clipFar, float clipNear, float horizontal_fov, const vec3 &in lookAt, const vec3 &in upVector, const vec3 &in position");

  Node::Component::registerAsBaseOfClass<CameraComponent>(angelScriptEngine, "CameraComponent");

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

