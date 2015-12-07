#include <glrt/debugging/visualization-renderer.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace debugging {


VisualizationRenderer VisualizationRenderer::debugSceneCameras(scene::Scene* scene)
{
  return VisualizationRenderer(scene, [scene](){
    if(scene->sceneCameras().isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawCameras(scene->sceneCameras().values());
  });
}

VisualizationRenderer VisualizationRenderer::debugSphereAreaLights(scene::Scene* scene)
{
  return VisualizationRenderer(scene, [scene](){
    if(scene->sphereAreaLights().isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawSphereAreaLights(scene->sphereAreaLights().values());
  });
}

VisualizationRenderer VisualizationRenderer::debugRectAreaLights(scene::Scene* scene)
{
  return VisualizationRenderer(scene, [scene](){
    if(scene->rectAreaLights().isEmpty())
      return DebugLineVisualisation::Ptr();
    else
      return debugging::DebugLineVisualisation::drawRectAreaLights(scene->rectAreaLights().values());
  });
}


VisualizationRenderer::VisualizationRenderer(scene::Scene* scene, const std::function<DebugLineVisualisation::Ptr()>& visualizationFactory)
  : factory(visualizationFactory)
{
  guiToggle.getter = [this]() -> bool {return !this->visualization.isNull();};
  guiToggle.setter = [this](bool show) {
    if(show)
      this->createVisualization();
    else
      this->visualization.clear();
  };

  connect(scene, &scene::Scene::sceneLoaded, this, &VisualizationRenderer::sceneLoaded);
}


void VisualizationRenderer::render()
{
  if(visualization)
    visualization->draw();
}


void VisualizationRenderer::createVisualization()
{
  visualization = factory();
}


void VisualizationRenderer::sceneLoaded()
{
  if(!visualization.isNull())
    createVisualization();
}


} // namespace debugging
} // namespace glrt

