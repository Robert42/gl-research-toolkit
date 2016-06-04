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

struct BoundingSphere
{
  glm::vec3 center;
  float radius;

  friend BoundingSphere operator*(const CoordFrame& frame, const BoundingSphere& sphere)
  {
    BoundingSphere s;
    s.center = frame.transform_point(sphere.center);
    s.radius = frame.scaleFactor * sphere.radius;
    return s;
  }
};

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
  typedef quint32 index_type;

  QVector<index_type> indices;
  QVector<Vertex> vertices;

  AABB boundingBox() const;
  BoundingSphere boundingSphere() const;
  void boundingShapes(BoundingSphere& boundignSphere, AABB& boundingBox) const;
  size_t rawDataSize() const;

  TriangleArray getTriangleArray() const;

  bool isIndexed() const;
  bool operator==(const StaticMesh& other) const;
  bool operator!=(const StaticMesh& other) const;

  static StaticMesh loadFromFile(const QString& filepath);
};


struct TriangleArray
{
  std::vector<glm::vec3> vertices;

  void applyTransformation(const CoordFrame& frame);
  void invertNormals();

  AABB boundingBox() const;
  BoundingSphere boundingSphere() const;
  void boundingShapes(BoundingSphere& boundignSphere, AABB& boundingBox) const;

  void operator+=(const TriangleArray& other);

  glm::vec3& operator[](size_t i);
  const glm::vec3& operator[](size_t i) const;

private:
};


} // namespace resources
} // namespace scene
} // namespace glrt

#include "static-mesh.inl"

#endif // GLRT_SCENE_RESOURCES_STATICMESH_H
