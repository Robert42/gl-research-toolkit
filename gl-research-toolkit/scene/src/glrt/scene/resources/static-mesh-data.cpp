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

  void loadStaticMeshImpl(const StaticMeshUuid&, const StaticMeshData::index_type* indices, size_t numIndices, const StaticMeshData::Vertex* vertices, size_t numVertices) override
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
  loader.loadStaticMesh(StaticMeshUuid(QUuid::createUuid()), filepath.toStdString());
  return loader.data;
}

} // namespace resources
} // namespace scene
} // namespace glrt

