#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>
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

DebugRenderer VisualizationRenderer::debugVoxelGrids(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    QHash<QString, scene::VoxelBoundingBox> voxelBoundingBoxes = scene::collectNamedVoxelGridSize(scene);
    if(voxelBoundingBoxes.isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawVoxelGrids(voxelBoundingBoxes.values());
  });
}

DebugRenderer VisualizationRenderer::debugVoxelBoundingSpheres(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    QHash<QString, BoundingSphere> voxelBoundingSpheres = scene::collectNamedVoxelBoundingSphere(scene);
    if(voxelBoundingSpheres.isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawSpheres(voxelBoundingSpheres.values());
  });
}

DebugRenderer VisualizationRenderer::showSceneBVH(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    const uint16_t n = scene->data->voxelGrids->length;
    if(n > 0)
      return debugging::DebugLineVisualisation::drawBvh(n);
    else
      return nullptr;
  });
}

DebugRenderer VisualizationRenderer::showSceneBVH_Grid(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    return debugging::DebugLineVisualisation::drawBvh_Grid(3);
  });
}

DebugRenderer VisualizationRenderer::showSceneAABB(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    return debugging::DebugLineVisualisation::drawBoundingBoxes({scene->aabb});
  });
}

DebugRenderer VisualizationRenderer::showMeshAABBs(scene::Scene* scene)
{
  return DebugRenderer(scene, [scene]() -> DebugRenderer::Implementation* {
    QVector<scene::AABB> boundingBoxes = scene::collectBoundingBoxes(scene);
    if(boundingBoxes.isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawBoundingBoxes(boundingBoxes);
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

DebugRenderer VisualizationRenderer::debugCones(QVector<Cone>* cones)
{
  return DebugRenderer([cones]() -> DebugRenderer::Implementation* {
    if(cones->isEmpty())
      return nullptr;
    else
      return debugging::DebugLineVisualisation::drawCones(*cones);
  });
}

DebugRenderer VisualizationRenderer::showWorldGrid()
{
  return DebugRenderer([]() -> DebugRenderer::Implementation* {
    return debugging::DebugLineVisualisation::drawWorldGrid();
  });
}

DebugRenderer VisualizationRenderer::showUniformTest()
{
  return DebugRenderer([]() -> DebugRenderer::Implementation* {
    return debugging::DebugLineVisualisation::drawUniformTest();
  });
}



} // namespace debugging
} // namespace renderer
} // namespace glrt

