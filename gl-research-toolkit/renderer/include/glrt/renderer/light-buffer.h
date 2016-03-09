#ifndef GLRT_RENDERER_LIGHTBUFFER_H
#define GLRT_RENDERER_LIGHTBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/renderer/simple-shader-storage-buffer.h>

namespace glrt {
namespace renderer {

class LightBuffer
{
public:
  struct LightData
  {
    GLuint64 sphereAreaLightsBuffer;
    GLuint64 rectAreaLightsBuffer;
    quint32 numSphereLights;
    quint32 numRectLights;
  };

  LightBuffer(scene::Scene& scene);
  ~LightBuffer();

  const LightData& updateLightData();

private:
  SimpleShaderStorageBuffer<scene::SphereAreaLightComponent> sphereAreaShaderStorageBuffer;
  SimpleShaderStorageBuffer<scene::RectAreaLightComponent> rectAreaShaderStorageBuffer;

  LightData _lightData;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_LIGHTBUFFER_H
