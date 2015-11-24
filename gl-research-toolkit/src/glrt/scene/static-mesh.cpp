#include <glrt/scene/static-mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {


StaticMesh::StaticMesh(gl::Buffer&& buffer)
  : buffer(std::move(buffer))
{
}

StaticMesh::StaticMesh(StaticMesh&& mesh)
  : buffer(std::move(mesh.buffer))
{
}


StaticMesh StaticMesh::loadMeshFromFile(const QString& filename)
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(filename.toStdString(),
                                           aiProcess_RemoveComponent |  // TODO: use this
                                           aiProcess_JoinIdenticalVertices | // Use Index Buffer
                                           aiProcess_PreTransformVertices | // As we are loading everything into one mesh
                                           aiProcess_CalcTangentSpace |
                                           aiProcess_ValidateDataStructure |
                                           aiProcess_RemoveRedundantMaterials |
                                           aiProcess_OptimizeMeshes |
                                           aiProcess_OptimizeGraph |
                                           aiProcess_ImproveCacheLocality |
                                           aiProcess_FindDegenerates  |
                                           aiProcess_FindInvalidData  |
                                           aiProcess_GenNormals | // If there are no normals, generate them
                                           aiProcess_GenUVCoords  | // If there are no UVs auto generate some replacement
                                           aiProcess_SortByPType  | // splits meshes with multiple primitive types into multiple meshes. This way we don't have to check, face is a line or a point
                                           aiProcess_Triangulate // Triangulare quads into triangles
                                           );

  if(!scene)
    throw GLRT_EXCEPTION(QString("Couldn't load mesh: %0").arg(importer.GetErrorString()));

  if(!scene->HasMeshes())
    throw GLRT_EXCEPTION(QString("Couldn't find any mesh in %0").arg(filename));

  quint32 numVertices = 0;
  quint32 numFaces = 0;

  for(quint32 i=0; i<scene->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[i];

    // aiProcess_SortByPType guarants to contain only one type, so we can expect it to contain only one type-bit
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
      continue;

    numVertices += mesh->mNumVertices;
    numFaces += mesh->mNumFaces;
  }

  if(numVertices == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any vertices in %0").arg(filename));
  if(numFaces == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any faces in %0").arg(filename));

  gl::Buffer vertexBuffer(numVertices, gl::Buffer::UsageFlag::IMMUTABLE);

  return std::move(StaticMesh(std::move(vertexBuffer)));
}


} // namespace scene
} // namespace glrt

