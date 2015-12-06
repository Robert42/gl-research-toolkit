#include <glrt/scene/camera-parameter.h>
#include <glrt/toolkit/assimp-glm-converter.h>
#include <glrt/toolkit/geometry.h>

#include <assimp/scene.h>

namespace glrt {
namespace scene {

CameraParameter CameraParameter::fromAssimp(const aiCamera& assimpCamera)
{
  CameraParameter camera;

  camera.horizontal_fov = assimpCamera.mHorizontalFOV;
  camera.aspect = assimpCamera.mAspect;
  camera.clipNear = assimpCamera.mClipPlaneNear;
  camera.clipFar = assimpCamera.mClipPlaneFar;

  camera.lookAt = to_glm_vec3(assimpCamera.mLookAt);
  camera.upVector = to_glm_vec3(assimpCamera.mUp);
  camera.position = to_glm_vec3(assimpCamera.mPosition);

  return camera;
}

CameraParameter CameraParameter::defaultDebugCamera()
{
  CameraParameter camera;

  camera.lookAt = glm::vec3(0, 1, 0);
  camera.upVector = glm::vec3(0, 0, 1);
  camera.position = glm::vec3(0, -5, 0);

  return camera;
}

glm::mat4 CameraParameter::inverseViewMatrix() const
{
  return glm::mat4(glm::vec4(glm::cross(this->lookAt, this->upVector), 0),
                   glm::vec4(this->upVector, 0),
                   glm::vec4(-this->lookAt, 0),
                   glm::vec4(this->position, 1));
}

glm::mat4 CameraParameter::viewMatrix() const
{
  return glm::affineInverse(inverseViewMatrix());
}

glm::mat4 CameraParameter::projectionMatrix() const
{
  return projectionMatrix(this->aspect);
}

glm::mat4 CameraParameter::projectionMatrix(float aspectRatio) const
{
  return glm::perspective<float>(this->horizontal_fov/aspectRatio,
                                 aspectRatio,
                                 this->clipNear,
                                 this->clipFar);
}

glm::mat4 CameraParameter::projectionMatrix(int width, int height) const
{
  return projectionMatrix(float(width) / float(height));
}

CameraParameter operator*(const glm::mat4& t, CameraParameter camera)
{
  camera.lookAt = transform_direction(t, camera.lookAt);
  camera.upVector = transform_direction(t, camera.upVector);
  camera.position = transform_point(t, camera.position);
  return camera;
}

} // namespace scene
} // namespace glrt

