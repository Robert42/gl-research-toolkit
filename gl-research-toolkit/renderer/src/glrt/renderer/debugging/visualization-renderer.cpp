#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/collect-scene-data.h>

namespace glrt {
namespace renderer {
namespace debugging {


DebugRenderer VisualizationRenderer::debugSceneCameras(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    QHash<QString, scene::CameraParameter> cameras = scene::collectNamedCameraParameters(scene);
    if(cameras.isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawCameras(cameras.values());
  });
}

DebugRenderer VisualizationRenderer::debugSphereAreaLights(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    QHash<QString, scene::SphereAreaLightComponent::Data> sphereAreaLight = scene::collectNamedSphereAreaLights(scene);
    if(sphereAreaLight.isEmpty())
                         return nullptr;
    else
      return debugging::DebugLineVisualisation::drawSphereAreaLights(sphereAreaLight.values());
  });
}

DebugRenderer VisualizationRenderer::debugRectAreaLights(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    QHash<QString, scene::RectAreaLightComponent::Data> rectAreaLights = scene::collectNamedRectAreaLights(scene);
    if(rectAreaLights.isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawRectAreaLights(rectAreaLights.values());
  });
}

DebugRenderer VisualizationRenderer::debugPoints(QVector<glm::vec3>* points)
{
  return DebugRenderer([points]() -> DebugRenderer::Implementation* {
    if(points->isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawPositions(*points);
  });
}

DebugRenderer VisualizationRenderer::debugArrows(QVector<Arrow>* arrows)
{
  return DebugRenderer([arrows]() -> DebugRenderer::Implementation* {
    if(arrows->isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawArrows(*arrows);
  });
}


} // namespace debugging
} // namespace renderer
} // namespace glrt

