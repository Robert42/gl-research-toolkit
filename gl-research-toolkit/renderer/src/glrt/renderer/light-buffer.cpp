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

  _lightData.numRectLights = rectAreaShaderStorageBuffer.numElements();
  _lightData.numSphereLights = sphereAreaShaderStorageBuffer.numElements();
  _lightData.rectAreaLightsBuffer = rectAreaShaderStorageBuffer.gpuBufferAddress();
  _lightData.sphereAreaLightsBuffer = sphereAreaShaderStorageBuffer.gpuBufferAddress();

  return _lightData;
}



} // namespace renderer
} // namespace glrt
