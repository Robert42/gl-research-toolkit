#ifndef GLRT_SCENE_CAMERAPARAMETER_H
#define GLRT_SCENE_CAMERAPARAMETER_H

#include <glrt/dependencies.h>

class aiCamera;

namespace glrt {
namespace scene {

struct Camera final
{
public:
  glm::vec3 lookAt = glm::vec3(0, 0, -1);
  float horizontal_fov = glm::radians(90.f);

  glm::vec3 upVector = glm::vec3(0, 1, 0);
  float aspect = 16.f/9.f;

  glm::vec3 position = glm::vec3(0);
  float clipNear = 0.001f;

  float clipFar = 100.f;
  padding<float, 3> _padding;

  static Camera fromAssimp(const aiCamera& camera);
  static Camera defaultDebugCamera();

  friend Camera operator*(const glm::mat4& t, Camera camera);

  glm::mat4 inverseViewMatrix() const;
  glm::mat4 viewMatrix() const;

  glm::mat4 projectionMatrix() const;
  glm::mat4 projectionMatrix(float aspectRatio) const;
  glm::mat4 projectionMatrix(int width, int height) const;
};

static_assert(sizeof(Camera)==64, "Please make sure the struct Camera is std140 compatible");

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_CAMERAPARAMETER_H
