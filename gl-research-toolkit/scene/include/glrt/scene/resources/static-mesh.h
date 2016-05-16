#ifndef GLRT_SCENE_RESOURCES_STATICMESH_H
#define GLRT_SCENE_RESOURCES_STATICMESH_H

#include <glrt/dependencies.h>
#include <glrt/scene/coord-frame.h>

namespace glrt {
namespace scene {
namespace resources {

struct AABB
{
  glm::vec3 minPoint;
  glm::vec3 maxPoint;
};

template<int stride>
struct TriangleArray;


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

  template<int n_Components=3>
  TriangleArray<n_Components> getTriangleArray() const;

  bool isIndexed() const;
  bool operator==(const StaticMesh& other) const;
  bool operator!=(const StaticMesh& other) const;

  static StaticMesh loadFromFile(const QString& filepath);
};


template<int n_Components=3>
struct TriangleArray
{
  static const int stride = sizeof(float) * n_Components;

  void applyTransformation(const CoordFrame& frame);
  void invertNormals();

  AABB boundingBox() const;

  void resize(int l);
  int length() const;

  template<int other_stride>
  void operator = (const TriangleArray<other_stride>& other);
  void operator += (const TriangleArray& other);

  glm::vec3& vertex(int i);
  const glm::vec3& vertex(int i) const;
  glm::vec3& operator[](int i);
  const glm::vec3& operator[](int i) const;

private:
  QVector<padding<float, n_Components>> positions;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#include "static-mesh.inl"

#endif // GLRT_SCENE_RESOURCES_STATICMESH_H
