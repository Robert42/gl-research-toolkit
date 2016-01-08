#include <glrt/scene/resources/static-mesh-data.h>
#include <glrt/scene/resources/resource-loader.h>

namespace glrt {
namespace scene {
namespace resources {

bool StaticMeshData::isIndexed() const
{
  return not indices.isEmpty();
}

class MyMeshLoader final : public StaticMeshLoader
{
public:
  StaticMeshData data;

  void loadStaticMeshImpl(const Uuid<StaticMeshData>&, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices) override
  {
    data.indices.resize(numIndices);
    data.vertices.resize(numVertices);
    memcpy(data.indices.data(), indices, numIndices*sizeof(StaticMeshData::index_type));
    memcpy(data.vertices.data(), vertices, numVertices*sizeof(StaticMeshData::Vertex));
  }
};

StaticMeshData StaticMeshData::loadFromFile(const QString& filepath)
{
  MyMeshLoader loader;
  loader.loadStaticMesh(Uuid<StaticMeshData>(QUuid::createUuid()), filepath.toStdString());
  return loader.data;
}

bool StaticMeshData::operator==(const StaticMeshData& other) const
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

bool StaticMeshData::Vertex::operator==(const Vertex& other) const
{
  return position==other.position
      && normal==other.normal
      && tangent==other.normal
      && uv==other.uv;
}

bool StaticMeshData::Vertex::isNearlyTheSame(const Vertex& other, float epsilon) const
{
  return glm::distance(position, other.position) <= epsilon
      && glm::distance(normal, other.normal) <= epsilon
      && glm::distance(tangent, other.tangent) <= epsilon
      && glm::distance(uv, other.uv) <= epsilon;
}

bool StaticMeshData::operator!=(const StaticMeshData& other) const
{
  return !(*this == other);
}

} // namespace resources
} // namespace scene
} // namespace glrt

