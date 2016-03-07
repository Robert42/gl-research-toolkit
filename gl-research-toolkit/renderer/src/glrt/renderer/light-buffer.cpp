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

  // #FIXME get the real data
  _lightData.numRectLights = 0;
  _lightData.numSphereLights = 0;
  _lightData.rectAreaLightsBuffer = 0;
  _lightData.sphereAreaLightsBuffer = 0;

  return _lightData;
}



} // namespace renderer
} // namespace glrt
