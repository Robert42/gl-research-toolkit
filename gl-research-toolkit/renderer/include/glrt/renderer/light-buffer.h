#ifndef GLRT_RENDERER_LIGHTBUFFER_H
#define GLRT_RENDERER_LIGHTBUFFER_H

#include <glrt/renderer/declarations.h>
#include <glrt/renderer/toolkit/managed-gl-buffer.h>
#include <glrt/scene/scene-data.h>
#include <glrt/scene/resources/light-source.h>

namespace glrt {
namespace renderer {

class LightBuffer
{
public:
  struct LightData
  {
    GLuint64 sphereAreaLightsBuffer;
    GLuint64 rectAreaLightsBuffer;
    GLuint64 sky_ibl_ggx;
    GLuint64 sky_ibl_diffuse;
    GLuint64 sky_ibl_cone_60;
    GLuint64 sky_ibl_cone_45;
    quint32 numSphereLights;
    quint32 numRectLights;
    padding<quint32, 2> _padding;
  };

  LightBuffer(scene::Scene& scene);
  ~LightBuffer();

  const LightData& updateLightData();

  quint32 numVisibleSphereAreaLights() const{return sphereLights.length;}
  quint32 numVisibleRectAreaLights() const{return rectLights.length;}

  bool needRerecording() const;

private:
  typedef scene::resources::LightSource::SphereAreaLight SphereAreaLight;
  typedef scene::resources::LightSource::RectAreaLight RectAreaLight;

  scene::Scene::Data::SphereLights& sphereLights;
  scene::Scene::Data::RectLights& rectLights;

  ManagedGLBuffer<SphereAreaLight> sphereLightUniforms;
  ManagedGLBuffer<RectAreaLight> rectLightUniforms;

  LightData _lightData;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_LIGHTBUFFER_H
