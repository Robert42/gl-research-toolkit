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


StaticMesh&& StaticMesh::loadMeshFromFile(const QString& filename)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename.toStdString(),
                                           // aiProcess_RemoveComponent |  TODO: use this
                                           aiProcess_JoinIdenticalVertices | // TODO: this can be used to allow switching between indexed and not indexed mode
                                           aiProcess_PreTransformVertices | // TODO: have this only activated when loading a single mesh
                                           aiProcess_CalcTangentSpace |
                                           aiProcess_GenNormals |
                                           aiProcess_ValidateDataStructure |
                                           aiProcess_RemoveRedundantMaterials |
                                           aiProcess_OptimizeMeshes |
                                           aiProcess_OptimizeGraph |
                                           aiProcess_ImproveCacheLocality |
                                           aiProcess_SortByPType  | // TODO Read the documentation to this
                                           aiProcess_FindDegenerates  |
                                           aiProcess_FindInvalidData  |
                                           aiProcess_GenUVCoords  | // TODO: think about this
                                           aiProcess_Triangulate
                                           );

  if(!scene)
    throw GLRT_EXCEPTION(QString("Couldn't load mesh: %0").arg(importer.GetErrorString()));

  if(!scene->HasMeshes())
    throw GLRT_EXCEPTION(QString("Couldn't find any mesh in %0").arg(filename));

  gl::Buffer buffer(42, gl::Buffer::UsageFlag::IMMUTABLE);

  return std::move(StaticMesh(std::move(buffer)));
}


} // namespace scene
} // namespace glrt

