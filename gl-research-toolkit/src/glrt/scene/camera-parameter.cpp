#include <glrt/scene/camera-parameter.h>
#include <glrt/toolkit/assimp-glm-converter.h>

#include <assimp/scene.h>

namespace glrt {
namespace scene {

void CameraParameter::loadFromAssimp(const aiCamera& camera)
{
  this->horizontal_fov = camera.mHorizontalFOV;
  this->aspect = camera.mAspect;
  this->clipNear = camera.mClipPlaneNear;
  this->clipFar = camera.mClipPlaneFar;

  this->lookAt = to_glm_vec3(camera.mLookAt);
  this->upVector = to_glm_vec3(camera.mUp);
  this->position = to_glm_vec3(camera.mPosition);
}


} // namespace scene
} // namespace glrt

