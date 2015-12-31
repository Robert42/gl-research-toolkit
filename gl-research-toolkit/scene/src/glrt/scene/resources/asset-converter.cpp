#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/static-mesh-data.h>
#include <glrt/toolkit/assimp-glm-converter.h>

#include <QTemporaryDir>
#include <QProcess>
#include <QDateTime>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {
namespace resources {

inline bool shouldConvert(const QFileInfo& targetFile, const QFileInfo& sourceFile)
{
  return !targetFile.exists() || targetFile.lastModified() < sourceFile.lastModified();
}

void convertStaticMesh_assimpToMesh(const QFileInfo& meshFile, const QFileInfo& sourceFile, bool indexed);

void runBlenderWithPythonScript(const QString& pythonScript, const QFileInfo& blenderFile);
QString python_exportSceneAsObjMesh(const QString& objFile);

void convertStaticMesh_BlenderToObj(const QFileInfo& meshFile, const QFileInfo& blenderFile, bool indexed)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to static mesh");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported.obj");

  runBlenderWithPythonScript(python_exportSceneAsObjMesh(tempFilePath), blenderFile);

  convertStaticMesh_assimpToMesh(meshFile, tempFilePath, indexed);
}

void convertStaticMesh(const std::string& meshFilename, const std::string& sourceFilename)
{
  bool indexed = true;// #TODO allow the script to choose, whether the mesh is indexed or not

  QFileInfo meshFile(QString::fromStdString(meshFilename));
  QFileInfo sourceFile(QString::fromStdString(sourceFilename));

  SPLASHSCREEN_MESSAGE(QString("Import static mesh <%0>").arg(sourceFile.fileName()));

  if(shouldConvert(meshFile, sourceFile))
  {
    if(sourceFile.suffix().toLower() == "blend")
      convertStaticMesh_BlenderToObj(meshFile, sourceFile, indexed);
    else
      convertStaticMesh_assimpToMesh(meshFile, sourceFile, indexed);
  }
}

void convertSceneGraph(const std::string& sceneGraphFilename, const std::string& sourceFilename)
{
  QFileInfo sceneGraphFile(QString::fromStdString(sceneGraphFilename));
  QFileInfo sourceFile(QString::fromStdString(sourceFilename));

  SPLASHSCREEN_MESSAGE(QString("Import static mesh <%0>").arg(sourceFile.fileName()));

  if(shouldConvert(sceneGraphFile, sourceFile))
  {
    qDebug() << "convertSceneGraph("<<sceneGraphFile.fileName()<<", "<<sourceFile.fileName()<<")";
  }
}


void runBlenderWithPythonScript(const QString& pythonScript, const QFileInfo& blenderFile)
{
  QString blenderProgram("blender"); // #TODO, search for blender instead of just assuming it in the PATH variable?
  QStringList arguments = {"--background", blenderFile.absoluteFilePath(), "--python-expr", pythonScript};

  if(QProcess::execute(blenderProgram, arguments) !=0)
    throw GLRT_EXCEPTION("Executing Blender failed");
}

QString to_python_string(QString str)
{
  return str.replace("\\", R"("'\\'")").replace("\"", "\\\"").prepend("r\"").append("\"");
}

QString python_exportSceneAsObjMesh(const QString& objFile)
{
  return QString("import bpy\n"
                 "bpy.ops.export_scene.obj("
                 "filepath=%0"
                 ", "
                 "check_existing=False"
                 ", "
                 "use_mesh_modifiers=True"
                 ", "
                 "global_scale=1.0"
                 ", "
                 "use_normals=True"
                 ", "
                 "use_uvs=True"
                 ", "
                 "use_materials=True"
                 ", "
                 "use_triangles=True"
                 ", "
                 "axis_forward='Y'"
                 ", "
                 "axis_up='Z'"
                 ")").arg(to_python_string(objFile));
}


typedef StaticMeshData::index_type index_type;
typedef StaticMeshData::Vertex Vertex;

StaticMeshData loadMeshFromAssimp(aiMesh** meshes, quint32 nMeshes, const glm::mat3& transformation, const QString& context, bool indexed);
void writeToScriptLoadingStaticMesh(QTextStream& stream, const QString& uuid, const StaticMeshData& data)
{
  static_assert(sizeof(index_type)==2, "For uint16 as array type to be correct, index_type must be also a 16bit integer");
  stream << "  array<uint16> indices";
  if(!data.isIndexed())
    stream << ";\n";
  else
  {
    stream << " = \n";
    stream << "  {";
    int j=0;
    for(index_type i : data.indices)
    {
      if(j!=0)
        stream << ",";
      if(j%16 == 0)
        stream << "\n    " << i;
      else
        stream << " " << i;
      j++;
    }
    stream << "\n  };\n";

    stream << "  array<float> vertexData = \n";
    stream << "  {";
    j=0;
    for(const Vertex& v : data.vertices)
    {
      if(j!=0)
        stream << ",";
      stream << "\n    ";

      stream << v.position[0] << ", ";
      stream << v.position[1] << ", ";
      stream << v.position[2] << ", ";
      stream << v.normal[0] << ", ";
      stream << v.normal[1] << ", ";
      stream << v.normal[2] << ", ";
      stream << v.tangent[0] << ", ";
      stream << v.tangent[1] << ", ";
      stream << v.tangent[2] << ", ";
      stream << v.uv[0] << ", ";
      stream << v.uv[1];

      j++;
    }

    stream << "\n  };\n";
  }
  stream << "  loader.loadStaticMesh("<<uuid<<", indices, vertexData);\n";
}

void convertStaticMesh_assimpToMesh(const QFileInfo& meshFile, const QFileInfo& sourceFile, bool indexed)
{
  qDebug() << "convertStaticMesh_assimpToMesh("<<meshFile.absoluteFilePath()<<","<<sourceFile.absoluteFilePath()<<")";

  Assimp::Importer importer;

  glm::mat3 transform = glm::mat3(1, 0, 0,
                                  0, 1, 0,
                                  0, 0, 1);
  QString sourceFilepath = sourceFile.filePath();

  const aiScene* scene = importer.ReadFile(sourceFilepath.toStdString(),
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
    throw GLRT_EXCEPTION(QString("Couldn't find any mesh in %0").arg(sourceFilepath));

  StaticMeshData data = loadMeshFromAssimp(scene->mMeshes, scene->mNumMeshes, transform, QString("\n(in file <%0>)").arg(sourceFilepath), indexed);

  QFile file(meshFile.absoluteFilePath());

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(meshFile.absoluteFilePath()));

  QTextStream outputStream(&file);

  outputStream << "void main(StaticMeshLoader@ loader, StaticMeshUuid &in uuid)\n{\n";
  writeToScriptLoadingStaticMesh(outputStream, "uuid", data);
  outputStream << "}";
}

StaticMeshData loadMeshFromAssimp(aiMesh** meshes, quint32 nMeshes, const glm::mat3& transform, const QString& context, bool indexed)
{
  quint32 numVertices = 0;
  quint32 numFaces = 0;

  for(quint32 i=0; i<nMeshes; ++i)
  {
    const aiMesh* mesh = meshes[i];

    // aiProcess_SortByPType guarants to contain only one type, so we can expect it to contain only one type-bit
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
      continue;

    if(!mesh->HasFaces())
      throw GLRT_EXCEPTION(QString("No Faces%0").arg(context));

    if(!mesh->HasNormals())
      throw GLRT_EXCEPTION(QString("No Normals%0").arg(context));

    if(!mesh->HasPositions())
      throw GLRT_EXCEPTION(QString("No Positions%0").arg(context));

    if(!mesh->HasTangentsAndBitangents())
      throw GLRT_EXCEPTION(QString("No Tangents%0").arg(context));

    // Quote http://learnopengl.com/?_escaped_fragment_=Advanced-Lighting/Normal-Mapping:
    // > Also important to realize is that aiProcess_CalcTangentSpace doesn't always work.
    // > Calculating tangents is based on texture coordinates and some model artists do certain
    // > texture tricks like mirroring a texture surface over a model by also mirroring half of
    // > the texture coordinates; this gives incorrect results when the mirroring is not taken
    // > into account (which Assimp doesn't).
    if(!mesh->HasTextureCoords(0))
      throw GLRT_EXCEPTION(QString("No Texture Coordinates. HINT: You probably forgot to create a uv-map%0").arg(context));

    numVertices += mesh->mNumVertices;
    numFaces += mesh->mNumFaces;
  }

  if(numVertices == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any vertices%0").arg(context));
  if(numFaces == 0)
    throw GLRT_EXCEPTION(QString("Couldn't find any faces%0").arg(context));

  if(numVertices > std::numeric_limits<index_type>::max())
    throw GLRT_EXCEPTION(QString("Too many vertices%0").arg(context));

  QVector<Vertex> vertices;
  QVector<index_type> indices;

  vertices.reserve(numVertices);
  indices.reserve(numFaces*3);

  for(quint32 i=0; i<nMeshes; ++i)
  {
    const aiMesh* mesh = meshes[i];

    // aiProcess_SortByPType guarants to contain only one type, so we can expect it to contain only one type-bit
    if(mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
      continue;

    index_type index_offset = vertices.size();

    for(quint32 j = 0; j<mesh->mNumVertices; ++j)
    {
      Vertex vertex;
      vertex.position = transform * to_glm_vec3(mesh->mVertices[j]);
      vertex.normal = transform * to_glm_vec3(mesh->mNormals[j]);
      vertex.tangent = transform * to_glm_vec3(mesh->mTangents[j]);
      vertex.uv = to_glm_vec3(mesh->mTextureCoords[0][j]).xy();

      vertices.push_back(vertex);
    }

    for(quint32 j = 0; j<mesh->mNumFaces; ++j)
    {
      const aiFace& face = mesh->mFaces[j];

      if(face.mNumIndices != 3)
        throw GLRT_EXCEPTION(QString("Unexpected non-triangle face in %0").arg(context));

      indices.push_back(face.mIndices[0]+index_offset);
      indices.push_back(face.mIndices[1]+index_offset);
      indices.push_back(face.mIndices[2]+index_offset);
    }
  }

  if(!indexed)
    indices.clear();

  return StaticMeshData{indices, vertices};
}

} // namespace resources
} // namespace scene
} // namespace glrt

