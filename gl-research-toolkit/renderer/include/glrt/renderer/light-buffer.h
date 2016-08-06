#ifndef GLRT_RENDERER_LIGHTBUFFER_H
#define GLRT_RENDERER_LIGHTBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/gl/command-list-recorder.h>

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
    padding<quint32, 2> _padding;
  };

  LightBuffer(scene::Scene& scene);
  ~LightBuffer();

  const LightData& updateLightData();

  // TODO:::::::::::::::::::::::::::
#if 0
  quint32 numVisibleRectAreaLights() const {return static_cast<quint32>(rectAreaShaderStorageBuffer.numElements());}
  quint32 numVisibleSphereAreaLights() const {return static_cast<quint32>(sphereAreaShaderStorageBuffer.numElements());}
  bool numVisibleChanged() const {return _numVisibleRectAreaLights!=numVisibleRectAreaLights() || _numVisibleSphereAreaLights!=numVisibleSphereAreaLights();}

  void updateNumberOfLights();
#else
  quint32 numVisibleRectAreaLights() const {return _numVisibleRectAreaLights;}
  quint32 numVisibleSphereAreaLights() const {return _numVisibleSphereAreaLights;}
#endif

  // TODO:::::::::::::::::::::::::::
#if 0
private:
  SimpleShaderStorageBuffer<scene::SphereAreaLightComponent> sphereAreaShaderStorageBuffer;
  SimpleShaderStorageBuffer<scene::RectAreaLightComponent> rectAreaShaderStorageBuffer;
#endif

  LightData _lightData;

  quint32 _numVisibleRectAreaLights = 0;
  quint32 _numVisibleSphereAreaLights = 0;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_LIGHTBUFFER_H
