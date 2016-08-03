#ifndef GLRT_SCENE_SCENE_DATA_H
#define GLRT_SCENE_SCENE_DATA_H

#include "scene.h"

namespace glrt {
namespace scene {

class Scene::Data
{
public:
  struct TransformData
  {
    int length;
    QVector<glm::vec3> position;
    QVector<glm::quat> orientation;
    QVector<float> scaleFactor;
    QVector<Node::Component*> component;
  };
  struct LightSourceData : public TransformData
  {
    QVector<glm::vec3> color;
    QVector<float> luminous_power;
    QVector<float> influence_radius;
  };

  struct SphereLightData : public LightSourceData
  {
    QVector<float> radius;
  };

  struct RectLightData : public LightSourceData
  {
    QVector<float> radius;
  };

  struct StaticMeshData : public TransformData
  {
    TransformData transformData;
  };

  TransformData emptyNodesStatic;
  TransformData emptyNodesDynamic;

  QVector<TransformData*> allTransformData;
  QVector<LightSourceData*> allLightSourceData;

  SphereLightData sphereLightData;
  RectLightData rectLightData;
  StaticMeshData staticMeshData;

  Data();
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_DATA_H
