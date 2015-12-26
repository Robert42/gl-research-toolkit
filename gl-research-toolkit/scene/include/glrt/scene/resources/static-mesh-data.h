#ifndef GLRT_SCENE_RESOURCES_STATICMESHDATA_H
#define GLRT_SCENE_RESOURCES_STATICMESHDATA_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {
namespace resources {

struct StaticMeshData
{
  QVector<int> indices;
  QVector<glm::vec3> positions;
  QVector<glm::vec3> normals;
  QVector<glm::vec3> tangents;
  QVector<glm::vec2> uvs;

  StaticMeshData();

  bool isIndexed() const;
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESHDATA_H
