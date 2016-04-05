#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/collect-scene-data.h>

namespace glrt {
namespace renderer {
namespace debugging {


VisualizationRenderer VisualizationRenderer::debugSceneCameras(scene::Scene* scene)
{
  return VisualizationRenderer(scene, [scene](){
    QHash<QString, scene::CameraParameter> cameras = scene::collectNamedCameraParameters(scene);
    if(cameras.isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawCameras(cameras.values());
  });
}

VisualizationRenderer VisualizationRenderer::debugSphereAreaLights(scene::Scene* scene)
{
  return VisualizationRenderer(scene, [scene](){
    QHash<QString, scene::SphereAreaLightComponent::Data> sphereAreaLight = scene::collectNamedSphereAreaLights(scene);
    if(sphereAreaLight.isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawSphereAreaLights(sphereAreaLight.values());
  });
}

VisualizationRenderer VisualizationRenderer::debugRectAreaLights(scene::Scene* scene)
{
  return VisualizationRenderer(scene, [scene](){
    QHash<QString, scene::RectAreaLightComponent::Data> rectAreaLights = scene::collectNamedRectAreaLights(scene);
    if(rectAreaLights.isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawRectAreaLights(rectAreaLights.values());
  });
}

VisualizationRenderer VisualizationRenderer::debugPoints(QVector<glm::vec3>* points)
{
  return VisualizationRenderer([points](){
    if(points->isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawPositions(*points);
  });
}

VisualizationRenderer VisualizationRenderer::debugArrows(QVector<Arrow>* arrows)
{
  return VisualizationRenderer([arrows](){
    if(arrows->isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawArrows(*arrows);
  });
}


VisualizationRenderer::VisualizationRenderer(const std::function<DebugLineVisualisation::Ptr()>& visualizationFactory)
  : VisualizationRenderer(nullptr, visualizationFactory)
{
}


VisualizationRenderer::VisualizationRenderer(scene::Scene* scene, const std::function<DebugLineVisualisation::Ptr()>& visualizationFactory)
  : DebugRenderer(scene),
    factory(visualizationFactory)
{
}

VisualizationRenderer::VisualizationRenderer(const VisualizationRenderer& other)
  : VisualizationRenderer(other.scene(), other.factory)
{
}

VisualizationRenderer::VisualizationRenderer(VisualizationRenderer&& other)
  : VisualizationRenderer(other.scene(), other.factory)
{
}

VisualizationRenderer::~VisualizationRenderer()
{
}


void VisualizationRenderer::render()
{
  if(visualization)
    visualization->draw();
}

void VisualizationRenderer::reinit()
{
  if(this->isEnabled())
    createVisualization();
  else
    visualization.clear();
}

void VisualizationRenderer::createVisualization()
{
  visualization = factory();
}



} // namespace debugging
} // namespace renderer
} // namespace glrt

