#include <glrt/glsl/layout-constants.h>

#include <glrt/renderer/scene-renderer.h>

namespace glrt {
namespace renderer {


LightBuffer::LightBuffer(glrt::scene::Scene& scene)
// TODO:::::::::::::::::::::::::::
#if 0
  : sphereAreaShaderStorageBuffer(scene),
    rectAreaShaderStorageBuffer(scene)
#endif
{
}

LightBuffer::~LightBuffer()
{
}

const LightBuffer::LightData& LightBuffer::updateLightData()
{
  // TODO:::::::::::::::::::::::::::
#if 0
  sphereAreaShaderStorageBuffer.update();
  rectAreaShaderStorageBuffer.update();

#endif
  _lightData.numRectLights = numVisibleRectAreaLights();
  _lightData.numSphereLights = numVisibleSphereAreaLights();
  // TODO:::::::::::::::::::::::::::
#if 0
  _lightData.rectAreaLightsBuffer = rectAreaShaderStorageBuffer.gpuBufferAddress();
  _lightData.sphereAreaLightsBuffer = sphereAreaShaderStorageBuffer.gpuBufferAddress();
#endif

  return _lightData;
}


// TODO:::::::::::::::::::::::::::
#if 0
void LightBuffer::updateNumberOfLights()
{
  _numVisibleRectAreaLights = numVisibleRectAreaLights();
  _numVisibleSphereAreaLights = numVisibleSphereAreaLights();
}
#endif


} // namespace renderer
} // namespace glrt
