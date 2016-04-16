#include <glrt/glsl/layout-constants.h>

#include <glrt/renderer/scene-renderer.h>

namespace glrt {
namespace renderer {


LightBuffer::LightBuffer(glrt::scene::Scene& scene)
  : sphereAreaShaderStorageBuffer(scene),
    rectAreaShaderStorageBuffer(scene)
{
}

LightBuffer::~LightBuffer()
{
}

const LightBuffer::LightData& LightBuffer::updateLightData()
{
  sphereAreaShaderStorageBuffer.update();
  rectAreaShaderStorageBuffer.update();

  _lightData.numRectLights = numVisibleRectAreaLights();
  _lightData.numSphereLights = numVisibleSphereAreaLights();
  _lightData.rectAreaLightsBuffer = rectAreaShaderStorageBuffer.gpuBufferAddress();
  _lightData.sphereAreaLightsBuffer = sphereAreaShaderStorageBuffer.gpuBufferAddress();

  return _lightData;
}


void LightBuffer::updateNumberOfLights()
{
  _numVisibleRectAreaLights = numVisibleRectAreaLights();
  _numVisibleSphereAreaLights = numVisibleSphereAreaLights();
}


} // namespace renderer
} // namespace glrt
