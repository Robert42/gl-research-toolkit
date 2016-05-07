#ifndef GLRT_SCENE_RESOURCES_STATICMESH_H
#define GLRT_SCENE_RESOURCES_STATICMESH_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {
namespace resources {

struct AABB
{
  glm::vec3 minPoint;
  glm::vec3 maxPoint;
};

struct StaticMesh
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 uv;

    bool operator==(const Vertex& other) const;
    bool isNearlyTheSame(const Vertex& other, float epsilon) const;

    void cleanUp();
  };
  typedef quint16 index_type;

  QVector<index_type> indices;
  QVector<Vertex> vertices;

  AABB boundingBox() const;
  size_t rawDataSize() const;

  bool isIndexed() const;
  bool operator==(const StaticMesh& other) const;
  bool operator!=(const StaticMesh& other) const;

  static StaticMesh loadFromFile(const QString& filepath);
};



} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_STATICMESH_H
