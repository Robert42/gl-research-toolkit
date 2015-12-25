#ifndef GLRT_RENDERER_DEBUGGING_VISUALIZATIONRENDERER_H
#define GLRT_RENDERER_DEBUGGING_VISUALIZATIONRENDERER_H

#include <glrt/gui/anttweakbar.h>
#include <glrt/debugging/debug-line-visualisation.h>

namespace glrt {
namespace renderer {
namespace debugging {


class VisualizationRenderer final
{
public:
  gui::TweakBarCBVar<bool> guiToggle;

  // Note, the given scene/vector instance must live longer than the returned instance
  static VisualizationRenderer debugSceneCameras(scene::Scene* scene);
  static VisualizationRenderer debugSphereAreaLights(scene::Scene* scene);
  static VisualizationRenderer debugRectAreaLights(scene::Scene* scene);
  static VisualizationRenderer debugPoints(QVector<glm::vec3>* points);
  static VisualizationRenderer debugArrows(QVector<Arrow>* arrows);

  VisualizationRenderer(const std::function<DebugLineVisualisation::Ptr()>& visualizationFactory);
  VisualizationRenderer(scene::Scene* scene, const std::function<DebugLineVisualisation::Ptr()>& visualizationFactory);
  VisualizationRenderer(const VisualizationRenderer&);
  VisualizationRenderer(VisualizationRenderer&& other);
  ~VisualizationRenderer();

  VisualizationRenderer& operator=(const VisualizationRenderer&) = delete;
  VisualizationRenderer& operator=(VisualizationRenderer&&) = delete;

  void render();

  void setEnabled(bool enabled);
  bool isEnabled() const;

  void update();

private:
  scene::Scene* scene;
  QMetaObject::Connection loadSceneConnection;

  bool _enabled : 1;
  std::function<DebugLineVisualisation::Ptr()> factory;
  DebugLineVisualisation::Ptr visualization;

  void createVisualization();
};


} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_VISUALIZATIONRENDERER_H
