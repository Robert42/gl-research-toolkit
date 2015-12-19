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
  : scene(scene),
    _enabled(false),
    factory(visualizationFactory)
{
  guiToggle.getter = std::bind(&VisualizationRenderer::isEnabled, this);
  guiToggle.setter = std::bind(&VisualizationRenderer::setEnabled, this, std::placeholders::_1);

  if(scene)
    loadSceneConnection = QObject::connect(scene, &scene::Scene::sceneLoaded, std::bind(&VisualizationRenderer::update, this));
}

VisualizationRenderer::VisualizationRenderer(const VisualizationRenderer& other)
  : VisualizationRenderer(other.scene, other.factory)
{
  if(other.isEnabled())
    this->setEnabled(true);
}

VisualizationRenderer::VisualizationRenderer(VisualizationRenderer&& other)
  : VisualizationRenderer(other.scene, other.factory)
{
  if(other.isEnabled())
    this->setEnabled(true);
}

VisualizationRenderer::~VisualizationRenderer()
{
  QObject::disconnect(loadSceneConnection);
}


void VisualizationRenderer::render()
{
  if(visualization)
    visualization->draw();
}

void VisualizationRenderer::setEnabled(bool enabled)
{
  if(enabled == this->isEnabled())
    return;

  this->_enabled = enabled;

  update();
}

bool VisualizationRenderer::isEnabled() const
{
  return this->_enabled;
}

void VisualizationRenderer::update()
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
} // namespace glrt

