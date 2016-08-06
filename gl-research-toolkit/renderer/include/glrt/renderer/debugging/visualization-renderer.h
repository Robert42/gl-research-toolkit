#ifndef GLRT_RENDERER_DEBUGGING_VISUALIZATIONRENDERER_H
#define GLRT_RENDERER_DEBUGGING_VISUALIZATIONRENDERER_H

#include <glrt/renderer/debugging/debug-renderer.h>
#include <glrt/renderer/debugging/debug-line-visualisation.h>

namespace glrt {
namespace renderer {
namespace debugging {


class VisualizationRenderer final
{
public:
  VisualizationRenderer() = delete;

  // Note, the given scene/vector instance must live longer than the returned instance
  static DebugRenderer debugSceneCameras(scene::Scene* scene);
  static DebugRenderer debugSphereAreaLights(scene::Scene* scene);
  static DebugRenderer debugRectAreaLights(scene::Scene* scene);
  static DebugRenderer debugVoxelGrids(scene::Scene* scene);
  static DebugRenderer debugVoxelBoundingSpheres(scene::Scene* scene);
  static DebugRenderer showSceneAABB(scene::Scene* scene);
  static DebugRenderer showMeshAABBs(scene::Scene* scene);
  static DebugRenderer debugPoints(QVector<glm::vec3>* points);
  static DebugRenderer debugArrows(QVector<Arrow>* arrows);
  static DebugRenderer debugCones(QVector<Cone>* cones);
  static DebugRenderer showWorldGrid();
  static DebugRenderer showUniformTest();
};


} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_VISUALIZATIONRENDERER_H
