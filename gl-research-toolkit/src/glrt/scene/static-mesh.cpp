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

StaticMesh::StaticMesh(gl::Buffer* indexBuffer, gl::Buffer* vertexBuffer, int numberIndices, int numberVertices)
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
  mesh.numberIndices = 0;
  mesh.numberVertices = 0;
  mesh.indexBuffer = nullptr;
  mesh.vertexBuffer = nullptr;
}


StaticMesh::~StaticMesh()
{
  delete indexBuffer;
  delete vertexBuffer;
}


StaticMesh& StaticMesh::operator=(StaticMesh&& mesh)
{
  std::swap(mesh.indexBuffer, this->indexBuffer);
  std::swap(mesh.vertexBuffer, this->vertexBuffer);
  std::swap(mesh.numberIndices, this->numberIndices);
  std::swap(mesh.numberVertices, this->numberVertices);
  return *this;
}


bool StaticMesh::isValidFile(const QFileInfo& file, bool parseFile)
{
  if(!parseFile)
  {
    return file.suffix().toLower() == "obj" || file.suffix().toLower() == "stl";
  }

  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(file.absoluteFilePath().toStdString(),
                                           aiProcess_FindInvalidData |
                                           aiProcess_FindDegenerates |
                                           aiProcess_ValidateDataStructure);

  return scene!=nullptr && scene->HasMeshes();
}


StaticMesh StaticMesh::loadMeshFromFile(const QString& file, bool indexed)
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(file.toStdString(),
                                           (indexed ? aiProcess_JoinIdenticalVertices : 0) | // Use Index Buffer
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
    throw GLRT_EXCEPTION(QString("Couldn't find any mesh in %0").arg(file));

  quint32 numVertices = 0;
  quint32 numFaces = 0;

  for(quint32 i=0; i<scene->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[i];

    // aiProcess_SortByPType guarants to contain only one type, so we can expect it to contain only one type-bit
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
      continue;

    if(!mesh->HasFaces())
      throw GLRT_EXCEPTION(QString("No Faces").arg(file));

    if(!mesh->HasNormals())
      throw GLRT_EXCEPTION(QString("No Normals").arg(file));

    if(!mesh->HasPositions())
      throw GLRT_EXCEPTION(QString("No Positions").arg(file));

    if(!mesh->HasTangentsAndBitangents())
      throw GLRT_EXCEPTION(QString("No Tangents").arg(file));

    // Quote http://learnopengl.com/?_escaped_fragment_=Advanced-Lighting/Normal-Mapping:
    // > Also important to realize is that aiProcess_CalcTangentSpace doesn't always work.
    // > Calculating tangents is based on texture coordinates and some model artists do certain
    // > texture tricks like mirroring a texture surface over a model by also mirroring half of
    // > the texture coordinates; this gives incorrect results when the mirroring is not taken
    // > into account (which Assimp doesn't).
    if(!mesh->HasTextureCoords(0))
      throw GLRT_EXCEPTION(QString("No Texture Coordinates. HINT: You probably forgot to create a uv-map").arg(file));

    numVertices += mesh->mNumVertices;
    numFaces += mesh->mNumFaces;
  }

  if(numVertices == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any vertices in %0").arg(file));
  if(numFaces == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any faces in %0").arg(file));

  if(numVertices > std::numeric_limits<index_type>::max())
    throw GLRT_EXCEPTION(QString("Too many vertices").arg(file));

  std::vector<Vertex> vertices;
  std::vector<index_type> indices;

  vertices.reserve(numVertices);
  indices.reserve(numFaces*3);

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

    index_type index_offset = indices.size();

    for(quint32 j = 0; j<mesh->mNumFaces; ++j)
    {
      const aiFace& face = mesh->mFaces[j];

      if(face.mNumIndices != 3)
        throw GLRT_EXCEPTION(QString("Unexpected non-triangle face in %0").arg(file));

      indices.push_back(face.mIndices[0]+index_offset);
      indices.push_back(face.mIndices[1]+index_offset);
      indices.push_back(face.mIndices[2]+index_offset);
    }
  }

  return createIndexed(indices.data(), indices.size(), vertices.data(), vertices.size(), indexed);
}


StaticMesh StaticMesh::createIndexed(const index_type* indices, int numIndices, const StaticMesh::Vertex* vertices, int numVertices, bool indexed)
{
  if(!indexed)
    return _createAsArray(indices, numIndices, vertices, numVertices);

  gl::Buffer* indexBuffer = new gl::Buffer(numIndices*sizeof(index_type), gl::Buffer::UsageFlag::IMMUTABLE, indices);
  gl::Buffer* vertexBuffer = new gl::Buffer(numVertices*sizeof(Vertex), gl::Buffer::UsageFlag::IMMUTABLE, vertices);

  return StaticMesh(indexBuffer,
                    vertexBuffer,
                    numIndices,
                    numVertices);
}


StaticMesh StaticMesh::_createAsArray(const index_type* indices, int numIndices, const StaticMesh::Vertex* vertices, int numVertices)
{
  std::vector<StaticMesh::Vertex> newVertices;

  newVertices.reserve(numIndices);

  for(int i=0; i<numIndices; ++i)
  {
    Q_ASSERT(indices[i] < numVertices);

    newVertices.push_back(vertices[indices[i]]);
  }

  Q_UNUSED(numVertices);

  return createAsArray(newVertices.data(), newVertices.size());
}


StaticMesh StaticMesh::createAsArray(const StaticMesh::Vertex* vertices, int numVertices)
{
  gl::Buffer* indexBuffer = nullptr;
  int numIndices = 0;

  gl::Buffer* vertexBuffer = new gl::Buffer(numVertices*sizeof(Vertex), gl::Buffer::UsageFlag::IMMUTABLE, vertices);

  return StaticMesh(indexBuffer,
                    vertexBuffer,
                    numIndices,
                    numVertices);
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
  if(indexBuffer != nullptr)
    indexBuffer->BindIndexBuffer();
  vertexBuffer->BindVertexBuffer(vertexBufferBinding, 0, vertexArrayObject.GetVertexStride(vertexBufferBinding));
}

void StaticMesh::draw(GLenum mode) const
{
  Q_ASSERT(sizeof(index_type) == 2); // assert, that GL_UNSIGNED_SHORT is the right type
  if(indexBuffer != nullptr)
    GL_CALL(glDrawElements, mode, numberIndices, GL_UNSIGNED_SHORT, nullptr);
  else
    GL_CALL(glDrawArrays, mode, 0, numberVertices);
}


} // namespace scene
} // namespace glrt

