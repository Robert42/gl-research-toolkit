#ifndef GLRT_SCENE_RESOURCES_STATICMESHDATA_H
#define GLRT_SCENE_RESOURCES_STATICMESHDATA_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {
namespace resources {

struct StaticMeshData
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 uv;

    bool operator==(const Vertex& other) const;
  };
  typedef quint16 index_type;

  QVector<index_type> indices;
  QVector<Vertex> vertices;

  bool isIndexed() const;
  bool operator==(const StaticMeshData& other) const;
  bool operator!=(const StaticMeshData& other) const;

  static StaticMeshData loadFromFile(const QString& filepath);
};



} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESHDATA_H
