#ifndef GLRT_SCENE_CAMERACOMPONENT_H
#define GLRT_SCENE_CAMERACOMPONENT_H

#include "node.h"
#include "camera.h"

namespace glrt {
namespace scene {

class CameraComponent final : public Node::Component
{
public:
  Camera cameraParameter;

  CameraComponent(Node& node, Node::Component* parent, const Uuid<CameraComponent>& uuid, const Camera& cameraParameter);
  ~CameraComponent();

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_CAMERACOMPONENT_H
