#ifndef GLRT_DEBUGGING_VISUALIZATIONRENDERER_H
#define GLRT_DEBUGGING_VISUALIZATIONRENDERER_H

#include <glrt/gui/anttweakbar.h>
#include <glrt/debugging/debug-line-visualisation.h>

namespace glrt {
namespace debugging {


class VisualizationRenderer : public QObject
{
  Q_OBJECT
public:
  gui::TweakBarCBVar<bool> guiToggle;

  // Note, the given scene instance must live longer than the returned instance
  static VisualizationRenderer debugSceneCameras(scene::Scene* scene);

  VisualizationRenderer(scene::Scene* scene, const std::function<DebugLineVisualisation::Ptr()>& visualizationFactory);
  VisualizationRenderer(VisualizationRenderer&& other);

  VisualizationRenderer(const VisualizationRenderer&) = delete;
  VisualizationRenderer& operator=(const VisualizationRenderer&) = delete;
  VisualizationRenderer& operator=(VisualizationRenderer&&) = delete;

  void render();

private:
  std::function<DebugLineVisualisation::Ptr()> factory;
  DebugLineVisualisation::Ptr visualization;

  void createVisualization();

private slots:
  void sceneLoaded();
};


} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_VISUALIZATIONRENDERER_H
