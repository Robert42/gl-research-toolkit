#include <glrt/glsl/layout-constants.h>

#include <glrt/renderer/scene-renderer.h>
#include <glrt/toolkit/profiler.h>

namespace glrt {
namespace renderer {


LightBuffer::LightBuffer(glrt::scene::Scene& scene)
  : sphereLights(scene.data->sphereLights),
    rectLights(scene.data->rectLights),
    sphereLightUniforms(sphereLights.capacity()),
    rectLightUniforms(rectLights.capacity())
{
}

LightBuffer::~LightBuffer()
{
}


const LightBuffer::LightData& LightBuffer::updateLightData()
{
  PROFILE_SCOPE("LightBuffer::updateLightData()")

  if(Q_UNLIKELY(rectLights.numDynamic+sphereLights.numDynamic>0 || sphereLights.dirtyOrder || rectLights.dirtyOrder))
  {
    if(Q_UNLIKELY(sphereLights.dirtyOrder))
    {
      // PERFORMANCE: sort by zindex for better cache utilzation
    }
    if(Q_UNLIKELY(rectLights.dirtyOrder))
    {
      // PERFORMANCE: sort by zindex for better cache utilzation
    }
    sphereLights.dirtyOrder = false;
    rectLights.dirtyOrder = false;

    SphereAreaLight* sphereLightBuffer = sphereLightUniforms.Map(sphereLights.length);
   #pragma omp simd
    for(quint32 i=0; i<sphereLights.length; ++i)
    {
      sphereLightBuffer[i].areaLightCommon.color = sphereLights.lightData[i].color;
      sphereLightBuffer[i].areaLightCommon.luminous_power = sphereLights.lightData[i].luminous_power;
      sphereLightBuffer[i].areaLightCommon.origin = sphereLights.position[i];
      sphereLightBuffer[i].areaLightCommon.influence_radius = sphereLights.lightData[i].influence_radius;
      sphereLightBuffer[i].radius = sphereLights.globalRadius(i);
    }
    sphereLightUniforms.Unmap();

    RectAreaLight* rectLightBuffer = rectLightUniforms.Map(rectLights.length);
   #pragma omp simd
    for(quint32 i=0; i<rectLights.length; ++i)
    {
      rectLightBuffer[i].areaLightCommon.color = rectLights.lightData[i].color;
      rectLightBuffer[i].areaLightCommon.luminous_power = rectLights.lightData[i].luminous_power;
      rectLightBuffer[i].areaLightCommon.origin = rectLights.position[i];
      rectLightBuffer[i].areaLightCommon.influence_radius = rectLights.lightData[i].influence_radius;

      const glm::vec2 half_size = rectLights.globalHalfSize(i);

      rectLightBuffer[i].half_width = half_size.x;
      rectLightBuffer[i].half_height = half_size.y;
      rectLights.globalTangents(i, &rectLightBuffer[i].tangent1, &rectLightBuffer[i].tangent2);

    }
    rectLightUniforms.Unmap();

    _lightData.numRectLights = rectLights.length;
    _lightData.numSphereLights = sphereLights.length;

    _lightData.rectAreaLightsBuffer = rectLightUniforms.buffer.gpuBufferAddress();
    _lightData.sphereAreaLightsBuffer = sphereLightUniforms.buffer.gpuBufferAddress();
  }

  return _lightData;
}

bool LightBuffer::needRerecording() const
{
  return sphereLights.numDynamic>0 || sphereLights.dirtyOrder ||rectLights.numDynamic>0 || rectLights.dirtyOrder;
}

} // namespace renderer
} // namespace glrt
