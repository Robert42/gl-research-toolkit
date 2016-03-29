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
      && tangent==other.normal
      && uv==other.uv;
}

bool StaticMesh::Vertex::isNearlyTheSame(const Vertex& other, float epsilon) const
{
  return glm::distance(position, other.position) <= epsilon
      && glm::distance(normal, other.normal) <= epsilon
      && glm::distance(tangent, other.tangent) <= epsilon
      && glm::distance(uv, other.uv) <= epsilon;
}

bool StaticMesh::operator!=(const StaticMesh& other) const
{
  return !(*this == other);
}

} // namespace resources
} // namespace scene
} // namespace glrt

