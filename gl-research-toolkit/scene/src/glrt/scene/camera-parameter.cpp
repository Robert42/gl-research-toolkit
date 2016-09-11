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

glm::mat4 CameraParameter::inverseViewMatrix() const
{
  return glm::mat4(glm::vec4(this->rightVector(), 0),
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
  return glm::perspective<float>(vertical_fov(aspectRatio),
                                 aspectRatio,
                                 this->clipNear,
                                 this->clipFar);
}

glm::mat4 CameraParameter::projectionMatrix(int width, int height) const
{
  return projectionMatrix(float(width) / float(height));
}

float CameraParameter::vertical_fov(float aspectRatio) const
{
  return this->horizontal_fov/aspectRatio;
}

float CameraParameter::vertical_fov() const
{
  return vertical_fov(this->aspect);
}

glm::vec3 CameraParameter::rightVector() const
{
  return glm::cross(this->lookAt, this->upVector);
}

CameraParameter operator*(const CoordFrame& frame, CameraParameter camera)
{
  camera.lookAt = frame.transform_direction(camera.lookAt);
  camera.upVector = frame.transform_direction(camera.upVector);
  camera.position = frame.transform_point(camera.position);
  return camera;
}

} // namespace scene
} // namespace glrt

