#include <glrt/scene/resources/static-mesh.h>
#include <glrt/scene/resources/static-mesh-loader.h>

namespace glrt {
namespace scene {
namespace resources {

bool StaticMesh::isIndexed() const
{
  return not indices.isEmpty();
}

class MyMeshLoader final : public StaticMeshLoader
{
public:
  StaticMesh data;

  void removeUnusedStaticMeshes(QSet<Uuid<StaticMesh>> usedStaticMeshes) override
  {
    Q_UNUSED(usedStaticMeshes);
  }

  void loadStaticMeshImpl(const Uuid<StaticMesh>&, const StaticMesh::index_type* indices, size_t numIndices, const StaticMesh::Vertex* vertices, size_t numVertices) override
  {
    data.indices.resize(numIndices);
    data.vertices.resize(numVertices);
    memcpy(data.indices.data(), indices, numIndices*sizeof(StaticMesh::index_type));
    memcpy(data.vertices.data(), vertices, numVertices*sizeof(StaticMesh::Vertex));
  }

  bool isAlreadyLoaded(const Uuid<StaticMesh>& uuid) const override
  {
    Q_UNUSED(uuid);
    return false;
  }

  AABB aabbForAlreadyLoaded(const Uuid<StaticMesh>& uuid) const override
  {
    return AABB();
  }
};

StaticMesh StaticMesh::loadFromFile(const QString& filepath)
{
  MyMeshLoader loader;
  loader.loadStaticMesh(Uuid<StaticMesh>(QUuid::createUuid()), filepath.toStdString());
  return loader.data;
}

bool StaticMesh::operator==(const StaticMesh& other) const
{
  if(this->indices.length()!=other.indices.length() || this->vertices.length()!=other.vertices.length())
    return false;

  if(this->indices!=other.indices)
    return false;

  float epsilon = 1.e-4f;

  for(int i=0; i<this->vertices.length(); ++i)
  {
    if(!this->vertices[i].isNearlyTheSame(other.vertices[i], epsilon))
      return false;
  }

  return true;
}

bool StaticMesh::Vertex::operator==(const Vertex& other) const
{
  return position==other.position
      && normal==other.normal
      && tangent==other.tangent
      && bitangent==other.bitangent
      && uv==other.uv;
}

bool StaticMesh::Vertex::isNearlyTheSame(const Vertex& other, float epsilon) const
{
  return glm::distance(position, other.position) <= epsilon
      && glm::distance(normal, other.normal) <= epsilon
      && glm::distance(tangent, other.tangent) <= epsilon
      && glm::distance(bitangent, other.bitangent) <= epsilon
      && glm::distance(uv, other.uv) <= epsilon;
}

void StaticMesh::Vertex::cleanUp()
{
  tangent = glm::normalize(tangent);
  bitangent = glm::normalize(bitangent);
  normal = glm::normalize(normal);
}

bool StaticMesh::operator!=(const StaticMesh& other) const
{
  return !(*this == other);
}


AABB StaticMesh::boundingBox() const
{
  AABB aabb;
  aabb.minPoint = glm::vec3(INFINITY);
  aabb.maxPoint = glm::vec3(-INFINITY);

  for(const Vertex& vertex : vertices)
  {
    aabb.minPoint = glm::min(aabb.minPoint, vertex.position);
    aabb.maxPoint = glm::max(aabb.maxPoint, vertex.position);
  }

  return aabb;
}

BoundingSphere StaticMesh::boundingSphere() const
{
  AABB aabb;
  BoundingSphere boundingSphere;

  boundingShapes(boundingSphere, aabb);

  return boundingSphere;
}

void StaticMesh::boundingShapes(BoundingSphere& sphere, AABB& aabb) const
{
  aabb = boundingBox();
  sphere = BoundingSphere{(aabb.maxPoint+aabb.minPoint)*0.5f, 0};

  for(const Vertex& vertex : vertices)
  {
    sphere.radius = glm::max(sphere.radius, distance(vertex.position, sphere.center));
  }
}

size_t StaticMesh::rawDataSize() const
{
  return size_t(indices.length()) * sizeof(index_type) + size_t(vertices.length()) * sizeof(Vertex);
}


} // namespace resources
} // namespace scene
} // namespace glrt

