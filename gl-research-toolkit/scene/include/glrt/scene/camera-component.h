#ifndef GLRT_SCENE_CAMERACOMPONENT_H
#define GLRT_SCENE_CAMERACOMPONENT_H

#include <glrt/scene/node.h>
#include <glrt/scene/camera-parameter.h>

namespace glrt {
namespace scene {

class CameraComponent final : public Node::Component
{
public:
  CameraParameter cameraParameter;

  CameraComponent(Node& node, Node::Component* parent, const Uuid<CameraComponent>& uuid, const CameraParameter& cameraParameter);
  ~CameraComponent();

  CameraParameter globalCameraParameter() const;

  void set_localCoordFrame(const CoordFrame& coordFrame);

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_CAMERACOMPONENT_H
