#ifndef GLRT_SCENE_CAMERAPARAMETER_H
#define GLRT_SCENE_CAMERAPARAMETER_H

#include <glrt/dependencies.h>

class aiCamera;

namespace glrt {
namespace scene {

struct CameraParameter
{
public:
  float horizontal_fov = glm::radians(90.f);
  float aspect = 16.f/9.f;
  float clipNear = 100.f;
  float clipFar = 0.001f;

  glm::vec3 lookAt = glm::vec3(0, 0, -1);
  padding<float> _padding2;

  glm::vec3 upVector = glm::vec3(0, 1, 0);
  padding<float> _padding3;

  glm::vec3 position = glm::vec3(0);
  padding<float> _padding4;

  void loadFromAssimp(const aiCamera& camera);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_CAMERAPARAMETER_H
