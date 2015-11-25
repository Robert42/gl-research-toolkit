#include <glrt/scene/static-mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {

const GLuint vertexBufferBinding = 0;
const GLuint indexBufferBinding = 1;

inline glm::vec3 toGlm3(const aiVector3D& v)
{
  return glm::vec3(v.x, v.y, v.z);
}

inline glm::vec2 toGlm2(const aiVector3D& v)
{
  return glm::vec2(v.x, v.y);
}

inline glm::vec2 toGlm2(const aiVector2D& v)
{
  return glm::vec2(v.x, v.y);
}

StaticMesh::StaticMesh(gl::Buffer&& indexBuffer, gl::Buffer&& vertexBuffer, int numberIndices, int numberVertices)
  : indexBuffer(std::move(indexBuffer)),
    vertexBuffer(std::move(vertexBuffer)),
    numberIndices(numberIndices),
    numberVertices(numberVertices)
{
}

StaticMesh::StaticMesh(StaticMesh&& mesh)
  : indexBuffer(std::move(mesh.indexBuffer)),
    vertexBuffer(std::move(mesh.vertexBuffer)),
    numberIndices(mesh.numberIndices),
    numberVertices(mesh.numberVertices)
{
}


StaticMesh StaticMesh::loadMeshFromFile(const QString& filename)
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(filename.toStdString(),
                                           //aiProcess_RemoveComponent |  // TODO: use this
                                           //aiProcess_FlipUVs | // TODO: Decide about this
                                           aiProcess_JoinIdenticalVertices | // Use Index Buffer
                                           aiProcess_PreTransformVertices | // As we are loading everything into one mesh
                                           aiProcess_ValidateDataStructure |
                                           aiProcess_RemoveRedundantMaterials |
                                           aiProcess_OptimizeMeshes |
                                           aiProcess_OptimizeGraph |
                                           aiProcess_ImproveCacheLocality |
                                           aiProcess_FindDegenerates  |
                                           aiProcess_FindInvalidData  |
                                           aiProcess_CalcTangentSpace | // If there are no tangents, generate them
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

    if(!mesh->HasFaces())
      throw GLRT_EXCEPTION(QString("No Faces").arg(filename));

    if(!mesh->HasNormals())
      throw GLRT_EXCEPTION(QString("No Normals").arg(filename));

    if(!mesh->HasPositions())
      throw GLRT_EXCEPTION(QString("No Positions").arg(filename));

    if(!mesh->HasTangentsAndBitangents())
      throw GLRT_EXCEPTION(QString("No Tangents").arg(filename));

    // Quote http://learnopengl.com/?_escaped_fragment_=Advanced-Lighting/Normal-Mapping:
    // > Also important to realize is that aiProcess_CalcTangentSpace doesn't always work.
    // > Calculating tangents is based on texture coordinates and some model artists do certain
    // > texture tricks like mirroring a texture surface over a model by also mirroring half of
    // > the texture coordinates; this gives incorrect results when the mirroring is not taken
    // > into account (which Assimp doesn't).
    if(!mesh->HasTextureCoords(0))
      throw GLRT_EXCEPTION(QString("No Texture Coordinates. HINT: You probably forgot to create a uv-map").arg(filename));

    numVertices += mesh->mNumVertices;
    numFaces += mesh->mNumFaces;
  }

  if(numVertices == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any vertices in %0").arg(filename));
  if(numFaces == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any faces in %0").arg(filename));

  if(numVertices > std::numeric_limits<index_type>::max())
    throw GLRT_EXCEPTION(QString("Too many vertices").arg(filename));

  std::vector<Vertex> vertices;
  std::vector<index_type> indices;

  vertices.reserve(numVertices);
  indices.reserve(numFaces);

  index_type index_offset = 0;

  for(quint32 i=0; i<scene->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[i];

    // aiProcess_SortByPType guarants to contain only one type, so we can expect it to contain only one type-bit
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
      continue;

    for(quint32 j = 0; j<mesh->mNumVertices; ++j)
    {
      Vertex vertex;
      vertex.position = toGlm3(mesh->mVertices[j]);
      vertex.normal = toGlm3(mesh->mNormals[j]);
      vertex.tangent = toGlm3(mesh->mTangents[j]);
      vertex.uv = toGlm2(mesh->mTextureCoords[0][j]);

      vertices.push_back(vertex);
    }

    for(quint32 j = 0; j<mesh->mNumFaces; ++j)
    {
      const aiFace& face = mesh->mFaces[j];

      if(face.mNumIndices != 3)
        throw GLRT_EXCEPTION(QString("Unexpected non-triangle face in %0").arg(filename));

      indices.push_back(face.mIndices[j]+index_offset);
      indices.push_back(face.mIndices[j]+index_offset);
      indices.push_back(face.mIndices[j]+index_offset);
    }

    index_offset += mesh->mNumVertices;
  }

  gl::Buffer indexBuffer(numFaces, gl::Buffer::UsageFlag::IMMUTABLE, indices.data());
  gl::Buffer vertexBuffer(numVertices, gl::Buffer::UsageFlag::IMMUTABLE, vertices.data());

  return std::move(StaticMesh(std::move(indexBuffer),
                              std::move(vertexBuffer),
                              numFaces*3,
                              numVertices));
}


// Normally, only one instance of gl::VertexArrayObject should be needed, as it only defined the format, which is reusable.
// Quote (https://www.opengl.org/wiki/Vertex_Specification#Separate_attribute_format):
// Better yet, if you want to use the same format but move the buffer around, it only takes one function call; namely glBindVertexBuffer​ with a buffer binding of 0.
//
//
// in other words, you can do the following:
// 1. Bind vertex array object
// 2. Bind Buffer to 0
// 3. Draw
// 4. Bind other Buffer to 0
// 5. Draw
// 6. Bind other Buffer to 0
// 7. Draw
gl::VertexArrayObject StaticMesh::generateVertexArrayObject()
{
  typedef gl::VertexArrayObject::Attribute Attribute;

  return std::move(gl::VertexArrayObject({Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 3, vertexBufferBinding),
                                          Attribute(Attribute::Type::FLOAT, 2, vertexBufferBinding)}));
}

void StaticMesh::bind(const gl::VertexArrayObject& vertexArrayObject)
{
  indexBuffer.BindIndexBuffer();
  vertexBuffer.BindVertexBuffer(vertexBufferBinding, 0, vertexArrayObject.GetVertexStride(vertexBufferBinding));
}

void StaticMesh::resetBinding()
{
  // TODO
}

void StaticMesh::draw()
{
  Q_ASSERT(sizeof(index_type) == 2);
  //qDebug() << "switch back to GL_TRIANGLES"; // FIXME
  GL_CALL(glDrawElements, GL_LINE_STRIP, numberIndices, GL_UNSIGNED_SHORT, nullptr);
}


} // namespace scene
} // namespace glrt

