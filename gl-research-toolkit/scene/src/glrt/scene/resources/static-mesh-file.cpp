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
  header._reserved.clear();

  QMap<Uuid<StaticMesh>, StaticMesh> arrayMeshes;
  QMap<Uuid<StaticMesh>, StaticMesh> indexedMeshes;
  quint64 totalNumVertices = 0;
  quint64 totalNumIndices = 0;

  for(auto i=this->staticMeshes.begin(); i!=this->staticMeshes.end(); ++i)
  {
    const StaticMesh& mesh = i.value();

    if(i.value().isIndexed())
      indexedMeshes[i.key()] = mesh;
    else
      arrayMeshes[i.key()] = mesh;

    if(size_t(mesh.vertices.length()) * sizeof(StaticMesh::Vertex) > std::numeric_limits<int>::max())
      throw GLRT_EXCEPTION(QString("Too many vertices to in single mesh!").arg(filename.filePath()));
    if(size_t(mesh.indices.length()) * sizeof(StaticMesh::index_type) > std::numeric_limits<int>::max())
      throw GLRT_EXCEPTION(QString("Too many indices to in single mesh!").arg(filename.filePath()));

    totalNumVertices += quint64(mesh.vertices.length());
    totalNumIndices += quint64(mesh.indices.length());
  }

  if(arrayMeshes.size() > 65535)
    throw GLRT_EXCEPTION(QString("Too many array static meshes to save to file!").arg(filename.filePath()));
  if(indexedMeshes.size() > 65535)
    throw GLRT_EXCEPTION(QString("Too many indexed static meshes to save to file!").arg(filename.filePath()));

  header.numArrayMeshes = quint16(arrayMeshes.size());
  header.numIndexedMeshes = quint16(indexedMeshes.size());

  if(totalNumVertices > std::numeric_limits<quint32>::max())
    throw GLRT_EXCEPTION(QString("Too many vertices to save to file!").arg(filename.filePath()));
  if(totalNumIndices > std::numeric_limits<quint32>::max())
    throw GLRT_EXCEPTION(QString("Too many indices to save to file!").arg(filename.filePath()));

  writeValue(file, header);

  totalNumVertices = 0;
  totalNumIndices = 0;

  for(auto i=arrayMeshes.begin(); i!=arrayMeshes.end(); ++i)
  {
    const StaticMesh& mesh = i.value();

    ArrayMesh meshInfo;
    meshInfo.uuid = i.key();
    meshInfo.indexOfFirstVertex = quint32(totalNumVertices);
    meshInfo.numVertices = quint32(mesh.vertices.length());

    writeValue(file, meshInfo);

    totalNumVertices += quint64(meshInfo.numVertices);
  }

  for(auto i=indexedMeshes.begin(); i!=indexedMeshes.end(); ++i)
  {
    const StaticMesh& mesh = i.value();

    IndexedMesh meshInfo;
    meshInfo.uuid = i.key();
    meshInfo.indexOfFirstVertex = quint32(totalNumVertices);
    meshInfo.numVertices = quint32(mesh.vertices.length());
    meshInfo.firstIndex = quint32(totalNumIndices);
    meshInfo.numIndices = quint32(mesh.vertices.length());

    writeValue(file, meshInfo);

    totalNumIndices += quint64(meshInfo.numIndices);
    totalNumVertices += quint64(meshInfo.numVertices);
  }

  for(const StaticMesh& mesh : arrayMeshes)
    file.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertices.length() * int(sizeof(StaticMesh::Vertex)));
  for(const StaticMesh& mesh : indexedMeshes)
    file.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertices.length() * int(sizeof(StaticMesh::Vertex)));
  for(const StaticMesh& mesh : indexedMeshes)
    file.write(reinterpret_cast<const char*>(mesh.indices.data()), mesh.indices.length() * int(sizeof(StaticMesh::index_type)));
}

void StaticMeshFile::load(const QFileInfo& filename)
{
  staticMeshes.clear();

  QFile file(filename.filePath());

  if(!file.open(QFile::ReadOnly))
    throw GLRT_EXCEPTION(QString("Couldn't read mesh from file %0").arg(filename.filePath()));

  Header header = readValue<Header>(file);

  if(header.magicNumber != magicNumber())
    throw GLRT_EXCEPTION(QString("Trying to load a file, which isn't a mesh file").arg(filename.filePath()));

  if(file.size() != sizeof(Header) + sizeof(ArrayMesh)*header.numArrayMeshes + sizeof(IndexedMesh)*header.numIndexedMeshes + sizeof(StaticMesh::Vertex)*header.totalNumVertices + sizeof(StaticMesh::Vertex)*header.totalNumIndices)
    throw GLRT_EXCEPTION(QString("Filesize of %0 doesn't match with the content").arg(filename.filePath()));

  QVector<ArrayMesh> arrayMeshHeaders;
  QVector<IndexedMesh> indexedMeshHeaders;

  arrayMeshHeaders.resize(header.numArrayMeshes);
  indexedMeshHeaders.resize(header.numIndexedMeshes);

  file.read(reinterpret_cast<char*>(arrayMeshHeaders.data()), qint64(arrayMeshHeaders.length())*qint64(sizeof(ArrayMesh)));
  file.read(reinterpret_cast<char*>(indexedMeshHeaders.data()), qint64(indexedMeshHeaders.length())*qint64(sizeof(IndexedMesh)));

  quint32 prevVertexEnd = 0;
  for(const ArrayMesh& meshHeader : arrayMeshHeaders)
  {
    if(meshHeader.uuid.toQUuid().isNull())
      throw GLRT_EXCEPTION(QString("Null Uuid detected in the mesh file %0").arg(filename.filePath()));
    if(meshHeader.indexOfFirstVertex != prevVertexEnd)
      throw GLRT_EXCEPTION(QString("There are unused vertices in the mesh file %0").arg(filename.filePath()));
    if(meshHeader.numVertices%3 != 0)
      throw GLRT_EXCEPTION(QString("There are numbor of vertices within a not indexed staticMesh is not a multiple of 3 in the mesh file %0").arg(filename.filePath()));
    prevVertexEnd += meshHeader.numVertices;
  }
  quint32 prevIndexEnd = 0;
  for(const IndexedMesh& meshHeader : indexedMeshHeaders)
  {
    if(meshHeader.uuid.toQUuid().isNull())
      throw GLRT_EXCEPTION(QString("Null Uuid detected in the mesh file %0").arg(filename.filePath()));
    if(meshHeader.indexOfFirstVertex != prevVertexEnd)
      throw GLRT_EXCEPTION(QString("There are unused vertices in the mesh file %0").arg(filename.filePath()));
    if(meshHeader.firstIndex != prevIndexEnd)
      throw GLRT_EXCEPTION(QString("There are unused vertices in the mesh file %0").arg(filename.filePath()));
    if(meshHeader.numIndices%3 != 0)
      throw GLRT_EXCEPTION(QString("There are numbor of indices within a not indexed staticMesh is not a multiple of 3 in the mesh file %0").arg(filename.filePath()));
    prevVertexEnd += meshHeader.numVertices;
    prevIndexEnd += meshHeader.numIndices;
  }
  if(prevIndexEnd != header.totalNumIndices)
    throw GLRT_EXCEPTION(QString("Mismatch of number of used indices in the mesh file %0").arg(filename.filePath()));
  if(prevVertexEnd != header.totalNumVertices)
    throw GLRT_EXCEPTION(QString("Mismatch of number of used vertices in the mesh file %0").arg(filename.filePath()));

  for(const ArrayMesh& meshHeader : arrayMeshHeaders)
  {
    StaticMesh mesh;

    mesh.vertices.resize(int(meshHeader.numVertices));
    file.read(reinterpret_cast<char*>(mesh.vertices.data()), meshHeader.numVertices * sizeof(StaticMesh::Vertex));

    staticMeshes[meshHeader.uuid]  = mesh;
  }
  for(const IndexedMesh& meshHeader : indexedMeshHeaders)
  {
    StaticMesh mesh;

    mesh.vertices.resize(int(meshHeader.numVertices));
    file.read(reinterpret_cast<char*>(mesh.vertices.data()), meshHeader.numVertices * sizeof(StaticMesh::Vertex));

    staticMeshes[meshHeader.uuid]  = mesh;
  }
  for(const IndexedMesh& meshHeader : indexedMeshHeaders)
  {
    StaticMesh& mesh = staticMeshes[meshHeader.uuid];

    mesh.indices.resize(int(meshHeader.numIndices));
    file.read(reinterpret_cast<char*>(mesh.indices.data()), meshHeader.numIndices * sizeof(StaticMesh::index_type));
  }
}

quint64 StaticMeshFile::magicNumber()
{
  return magicNumberForString("glrtmesh");
}


} // namespace resources
} // namespace scene
} // namespace glrt
