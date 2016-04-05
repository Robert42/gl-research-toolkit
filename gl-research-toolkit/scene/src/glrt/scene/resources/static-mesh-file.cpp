#include <glrt/scene/resources/static-mesh-file.h>
#include <glrt/toolkit/plain-old-data-stream.h>

namespace glrt {
namespace scene {
namespace resources {

StaticMeshFile::StaticMeshFile()
{
}

void StaticMeshFile::save(const QFileInfo& filename)
{
  QFile file(filename.filePath());

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Couldn't write mesh to file %0").arg(filename.filePath()));

  Header header;
  header._padding.clear();

  if(staticMesh.vertices.length() > 65535)
    throw GLRT_EXCEPTION(QString("Couldn't write mesh with more than 65535 vertices to file %0").arg(filename.filePath()));
  if(staticMesh.vertices.length() == 0)
    throw GLRT_EXCEPTION(QString("Couldn't write mesh with zero vertices to file %0").arg(filename.filePath()));
  header.numVertices = quint16(staticMesh.vertices.length());
  header.numIndices = quint32(staticMesh.indices.length());
  writeValue(file, header);

  file.write(reinterpret_cast<const char*>(staticMesh.vertices.data()), staticMesh.vertices.length() * int(sizeof(StaticMesh::Vertex)));
  file.write(reinterpret_cast<const char*>(staticMesh.indices.data()), staticMesh.indices.length() * int(sizeof(StaticMesh::index_type)));
}

void StaticMeshFile::load(const QFileInfo& filename)
{
  QFile file(filename.filePath());

  if(size_t(file.size()) < sizeof(Header))
    throw GLRT_EXCEPTION(QString("Mesh file too small %0 with bytes").arg(filename.filePath()).arg(file.size()));

  if(!file.open(QFile::ReadOnly))
    throw GLRT_EXCEPTION(QString("Couldn't read mesh from file %0").arg(filename.filePath()));

  Header header = readValue<Header>(file);

  if(header.magicNumber != magicNumber())
    throw GLRT_EXCEPTION(QString("Trying to load a file, which isn't a mesh file").arg(filename.filePath()));

  if(header.headerLength != sizeof(Header))
    throw GLRT_EXCEPTION(QString("Invalid Mesh mesh file %0 (header size mismatch)").arg(filename.filePath()));

  if(header.vertexSize != sizeof(StaticMesh::Vertex))
    throw GLRT_EXCEPTION(QString("Invalid Mesh mesh file %0 (vertex size mismatch)").arg(filename.filePath()));

  if(header.indexedMeshSize != sizeof(StaticMesh::index_type))
    throw GLRT_EXCEPTION(QString("Invalid Mesh mesh file %0 (index size mismatch)").arg(filename.filePath()));

  if(header.numIndices > std::numeric_limits<int>::max())
    throw GLRT_EXCEPTION(QString("Too many indicesin file  %0").arg(filename.filePath()));

  if(file.size() != qint64(sizeof(Header)) + qint64(sizeof(StaticMesh::Vertex))*qint64(header.numVertices) + qint64(sizeof(StaticMesh::index_type))*qint64(header.numIndices))
    throw GLRT_EXCEPTION(QString("Filesize of %0 doesn't match with the content").arg(filename.filePath()));

  staticMesh.vertices.resize(header.numVertices);
  staticMesh.indices.resize(header.numIndices);

  file.read(reinterpret_cast<char*>(staticMesh.vertices.data()), header.numVertices * sizeof(StaticMesh::Vertex));
  file.read(reinterpret_cast<char*>(staticMesh.indices.data()), header.numIndices * sizeof(StaticMesh::index_type));
}

quint64 StaticMeshFile::magicNumber()
{
  return magicNumberForString("glrtmesh");
}

} // namespace resources
} // namespace scene
} // namespace glrt
