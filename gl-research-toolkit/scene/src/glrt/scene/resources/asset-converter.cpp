#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/static-mesh-data.h>
#include <glrt/scene/camera-parameter.h>
#include <glrt/toolkit/assimp-glm-converter.h>

#include <QTemporaryDir>
#include <QProcess>
#include <QRegularExpression>
#include <QDateTime>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;

inline bool shouldConvert(const QFileInfo& targetFile, const QFileInfo& sourceFile)
{
  return !targetFile.exists() || targetFile.lastModified() < sourceFile.lastModified();
}

void convertStaticMesh_assimpToMesh(const QFileInfo& meshFile, const QFileInfo& sourceFile, bool indexed);
void convertSceneGraph_assimpToSceneGraph(const QFileInfo& meshFile, const QFileInfo& sourceFile, const Uuid<ResourceGroup>& uuid, const SceneGraphImportSettings& settings);

void runBlenderWithPythonScript(const QString& pythonScript, const QFileInfo& blenderFile);
QString python_exportSceneAsObjMesh(const QString& objFile);
QString python_exportSceneAsColladaSceneGraph(const QString& objFile);

void convertStaticMesh_BlenderToObj(const QFileInfo& meshFile, const QFileInfo& blenderFile, bool indexed)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to static mesh");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported.obj");

  runBlenderWithPythonScript(python_exportSceneAsObjMesh(tempFilePath), blenderFile);

  convertStaticMesh_assimpToMesh(meshFile, tempFilePath, indexed);
}

void convertSceneGraph_BlenderToCollada(const QFileInfo& sceneGraphFile, const QFileInfo& blenderFile, const Uuid<ResourceGroup>& uuid, const SceneGraphImportSettings &settings)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to scene-graph");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported-scene.dae");

  runBlenderWithPythonScript(python_exportSceneAsColladaSceneGraph(tempFilePath), blenderFile);

  convertSceneGraph_assimpToSceneGraph(sceneGraphFile, tempFilePath, uuid, settings);
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

void convertSceneGraph(const QString& sceneGraphFilename, const QString& sourceFilename, const Uuid<ResourceGroup>& uuid, const SceneGraphImportSettings &settings)
{
  QFileInfo sceneGraphFile(sceneGraphFilename);
  QFileInfo sourceFile(sourceFilename);

  SPLASHSCREEN_MESSAGE(QString("Import scene graph <%0>").arg(sourceFile.fileName()));

  // #TODO uncomment
  //if(shouldConvert(sceneGraphFile, sourceFile))
  {
    qDebug() << "convertSceneGraph("<<sceneGraphFile.fileName()<<", "<<sourceFile.fileName()<<")";
    convertSceneGraph_BlenderToCollada(sceneGraphFile, sourceFile, uuid, settings);
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

QString python_exportSceneAsColladaSceneGraph(const QString& colladaFile)
{
  return QString("import bpy\n"
                 "bpy.ops.wm.collada_export("
                 "filepath=%0"
                 ", "
                 "apply_modifiers=True"
                 ", "
                 "triangulate=True"
                 ", "
                 "use_object_instantiation=True"
                 ", "
                 "sort_by_name=True"
                 ", "
                 "export_transformation_type_selection='matrix'"
                 ")").arg(to_python_string(colladaFile));
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

  outputStream << "void main(StaticMeshLoader@ loader, Uuid<StaticMesh> &in uuid)\n{\n";
  writeToScriptLoadingStaticMesh(outputStream, "uuid", data);
  outputStream << "}";
}

struct SceneGraphImportAssets
{
  QVector<Uuid<MaterialData>> materials;
  QVector<Uuid<CameraParameter>> cameras;
  QVector<Uuid<LightData>> lights;
  QVector<Uuid<StaticMeshData>> meshes;
  QHash<aiNode*, Uuid<Entity>> nodes;
  QHash<QUuid, QString> labels;
  bool indexed = true;
};

void convertSceneGraph_assimpToSceneGraph(const QFileInfo& sceneGraphFile, const QFileInfo& sourceFile, const Uuid<ResourceGroup>& uuid, const SceneGraphImportSettings &settings)
{
  // #FIXME: register the fallback
  const Uuid<MaterialData> fallbackMaterial("{a8f3fb1b-1168-433b-aaf8-e24632cce156}");
  const Uuid<LightData> fallbackLight("{893463c4-143a-406f-9ef7-3506817d5837}");

  SceneGraphImportAssets assets;

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(sourceFile.absoluteFilePath().toStdString(),
                                           (assets.indexed ? aiProcess_JoinIdenticalVertices : 0) | // Use Index Buffer
                                           aiProcess_ValidateDataStructure |
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
    throw GLRT_EXCEPTION(QString("Couldn't load scene: %0").arg(importer.GetErrorString()));

  QVector<aiNode*> allNodesToImport;
  QSet<aiCamera*> allCamerasToImport;
  QSet<aiMesh*> allMeshesToImport;

  assets.materials.resize(scene->mNumMaterials);
  assets.materials.fill(fallbackMaterial);
  for(quint32 i=0; i<scene->mNumMaterials; ++i)
  {
    aiString name;
    if(scene->mMaterials[i]->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
    {
      QString n = name.C_Str();
      if(settings.materialUuids.contains(n) || settings.materialUuids.contains(n.remove("-material")))
        assets.materials[i] = settings.materialUuids[n];
    }
  }

  assets.cameras.resize(scene->mNumCameras);
  for(quint32 i=0; i<scene->mNumCameras; ++i)
  {
    QString n = scene->mCameras[i]->mName.C_Str();
    if(settings.cameraUuids.contains(n) || settings.cameraUuids.contains(n.remove("-camera")))
      assets.cameras[i] = settings.cameraUuids[n];
    else
      assets.cameras[i] = Uuid<CameraParameter>(QUuid::createUuidV5(uuid, n));
    assets.labels[assets.cameras[i]] = n;

    if(settings.shouldImportCamera(n))
      allCamerasToImport.insert(scene->mCameras[i]);
  }

  // #FIXME: how to handle the same with different materials?
  assets.meshes.resize(scene->mNumMeshes);
  for(quint32 i=0; i<scene->mNumMeshes; ++i)
  {
    QString n = scene->mMeshes[i]->mName.C_Str();
    if(settings.meshUuids.contains(n) || settings.meshUuids.contains(n.remove("-mesh")))
      assets.meshes[i] = settings.meshUuids[n];
    else
      assets.meshes[i] = Uuid<StaticMeshData>(QUuid::createUuidV5(uuid, n));
    assets.labels[assets.meshes[i]] = n;

    if(settings.shouldImportMesh(n))
      allMeshesToImport.insert(scene->mMeshes[i]);
  }

  assets.lights.resize(scene->mNumLights);
  for(quint32 i=0; i<scene->mNumLights; ++i)
  {
    QString n = scene->mLights[i]->mName.C_Str();
    if(settings.lightUuids.contains(n) || settings.lightUuids.contains(n.remove("-light")))
      assets.lights[i] = settings.lightUuids[n];
    else
      assets.lights[i] = fallbackLight;
    assets.labels[assets.lights[i]] = n;
  }

  allNodesToImport.reserve(scene->mNumMeshes+scene->mNumLights+scene->mNumCameras+42);
  QStack<aiNode*> nodes;
  nodes.push(scene->mRootNode);
  while(!nodes.isEmpty())
  {
    aiNode* node = nodes.pop();

    for(quint32 i=0; i<node->mNumChildren; ++i)
      nodes.push(node->mChildren[i]);

    QString n = node->mName.C_Str();

    if(settings.nodeUuids.contains(n) || settings.nodeUuids.contains(n.remove("-node")))
      assets.nodes[node] = settings.nodeUuids[n];
    else
      assets.nodes[node] = Uuid<Entity>(QUuid::createUuidV5(uuid, n));

    if(settings.shouldImportNode(n))
      allNodesToImport.append(node);
  }

  // #TODO cameras
  // #TODO lights
  // #TODO nodes
  // #TODO meshes


  // #IMPLEMENT
  Q_UNUSED(sceneGraphFile);
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


void SceneGraphImportSettings::set_meshesToImport(AngelScript::CScriptArray* meshesToImport)
{
  this->meshesToImport = AngelScriptIntegration::scriptArrayToStringSet(meshesToImport);
  meshesToImport->Release();
}

void SceneGraphImportSettings::set_camerasToImport(AngelScript::CScriptArray* camerasToImport)
{
  this->camerasToImport = AngelScriptIntegration::scriptArrayToStringSet(camerasToImport);
  camerasToImport->Release();
}

void SceneGraphImportSettings::set_nodesToImport(AngelScript::CScriptArray* nodesToImport)
{
  this->nodesToImport = AngelScriptIntegration::scriptArrayToStringSet(nodesToImport);
  nodesToImport->Release();
}

void SceneGraphImportSettings::set_meshUuids(AngelScript::CScriptDictionary* meshUuids)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("QUuid");
  QSet<int> meshUuidTypeIds = {uuidTypeId, angelScriptEngine->GetTypeIdByDecl("Uuid<StaticMesh>")};

  this->meshUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<StaticMeshData>>(meshUuids, meshUuidTypeIds);

  meshUuids->Release();
}

void SceneGraphImportSettings::set_materialUuids(AngelScript::CScriptDictionary* materialUuids)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("QUuid");
  QSet<int> materialUuidTypeIds = {uuidTypeId, angelScriptEngine->GetTypeIdByDecl("Uuid<Material>")};

  this->materialUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<MaterialData>>(materialUuids, materialUuidTypeIds);

  materialUuids->Release();
}

void SceneGraphImportSettings::set_lightUuids(AngelScript::CScriptDictionary* lightUuids)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("QUuid");
  QSet<int> lightUuidTypeIds = {uuidTypeId, angelScriptEngine->GetTypeIdByDecl("Uuid<Light>")};

  this->lightUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<LightData>>(lightUuids, lightUuidTypeIds);

  lightUuids->Release();
}

void SceneGraphImportSettings::set_nodeUuids(AngelScript::CScriptDictionary* nodeUuids)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("QUuid");
  QSet<int> nodeUuidTypeIds = {uuidTypeId, angelScriptEngine->GetTypeIdByDecl("Uuid<Node>")};

  this->nodeUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<Entity>>(nodeUuids, nodeUuidTypeIds);

  nodeUuids->Release();
}

void SceneGraphImportSettings::set_cameraUuids(AngelScript::CScriptDictionary* cameraUuids)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("QUuid");
  QSet<int> lightUuidTypeIds = {uuidTypeId, angelScriptEngine->GetTypeIdByDecl("Uuid<Camera>")};

  this->cameraUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<CameraParameter>>(cameraUuids, lightUuidTypeIds);

  cameraUuids->Release();
}

AngelScript::CScriptArray* SceneGraphImportSettings::get_meshesToImport()
{
  return AngelScriptIntegration::scriptArrayFromStringSet(this->meshesToImport, angelScriptEngine);
}

AngelScript::CScriptArray* SceneGraphImportSettings::get_camerasToImport()
{
  return AngelScriptIntegration::scriptArrayFromStringSet(this->camerasToImport, angelScriptEngine);
}

AngelScript::CScriptArray* SceneGraphImportSettings::get_nodesToImport()
{
  return AngelScriptIntegration::scriptArrayFromStringSet(this->nodesToImport, angelScriptEngine);
}

AngelScript::CScriptDictionary* SceneGraphImportSettings::get_meshUuids()
{
  int meshUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<StaticMesh>");
  return AngelScriptIntegration::scriptDictionaryFromHash(this->meshUuids, meshUuidTypeId, angelScriptEngine);
}

AngelScript::CScriptDictionary* SceneGraphImportSettings::get_materialUuids()
{
  int materialUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Material>");
  return AngelScriptIntegration::scriptDictionaryFromHash(this->materialUuids, materialUuidTypeId, angelScriptEngine);
}

AngelScript::CScriptDictionary* SceneGraphImportSettings::get_lightUuids()
{
  int lightUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Light>");
  return AngelScriptIntegration::scriptDictionaryFromHash(this->lightUuids, lightUuidTypeId, angelScriptEngine);
}

AngelScript::CScriptDictionary* SceneGraphImportSettings::get_nodeUuids()
{
  int nodeUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Node>");
  return AngelScriptIntegration::scriptDictionaryFromHash(this->nodeUuids, nodeUuidTypeId, angelScriptEngine);
}

AngelScript::CScriptDictionary* SceneGraphImportSettings::get_cameraUuids()
{
  int cameraUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Camera>");
  return AngelScriptIntegration::scriptDictionaryFromHash(this->cameraUuids, cameraUuidTypeId, angelScriptEngine);
}

bool SceneGraphImportSettings::shouldImportMesh(const QString& name) const
{
  return shouldImport(name, meshesToImport) || shouldImport(name+"-mesh", meshesToImport);
}

bool SceneGraphImportSettings::shouldImportCamera(const QString& name) const
{
  return shouldImport(name, meshesToImport) || shouldImport(name+"-camera", meshesToImport);
}

bool SceneGraphImportSettings::shouldImportNode(const QString& name) const
{
  return shouldImport(name, meshesToImport) || shouldImport(name+"-node", meshesToImport);
}

bool SceneGraphImportSettings::shouldImport(const QString& name, const QSet<QString>& patternsToImport)
{
  for(const QString& pattern : patternsToImport)
  {
    QRegularExpression regex(pattern);

    if(regex.match(name).hasMatch())
      return true;
  }

  return false;
}

void SceneGraphImportSettings::registerType()
{
  int r;
  const char* name = "SceneGraphImportSettings";

  r = angelScriptEngine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF); AngelScriptCheck(r);

  AngelScriptIntegration::RefCountedObject::registerAsBaseOfClass<SceneGraphImportSettings>(angelScriptEngine, name);

  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_FACTORY, (std::string(name)+"@ f()").c_str(), AngelScript::asFUNCTION(SceneGraphImportSettings::create), AngelScript::asCALL_CDECL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_meshesToImport(array<string>@ meshesToImport)", AngelScript::asMETHOD(SceneGraphImportSettings,set_meshesToImport), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_camerasToImport(array<string>@ meshesToImport)", AngelScript::asMETHOD(SceneGraphImportSettings,set_camerasToImport), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_nodesToImport(array<string>@ meshesToImport)", AngelScript::asMETHOD(SceneGraphImportSettings,set_nodesToImport), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_meshUuids(dictionary@ meshUuids)", AngelScript::asMETHOD(SceneGraphImportSettings,set_meshUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_materialUuids(dictionary@ materialUuids)", AngelScript::asMETHOD(SceneGraphImportSettings,set_materialUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_lightUuids(dictionary@ lightUuids)", AngelScript::asMETHOD(SceneGraphImportSettings,set_lightUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_nodeUuids(dictionary@ lightUuids)", AngelScript::asMETHOD(SceneGraphImportSettings,set_nodeUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "void set_cameraUuids(dictionary@ lightUuids)", AngelScript::asMETHOD(SceneGraphImportSettings,set_cameraUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "array<string>@ get_meshesToImport()", AngelScript::asMETHOD(SceneGraphImportSettings,get_meshesToImport), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "array<string>@ get_camerasToImport()", AngelScript::asMETHOD(SceneGraphImportSettings,get_camerasToImport), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "array<string>@ get_nodesToImport()", AngelScript::asMETHOD(SceneGraphImportSettings,get_nodesToImport), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "dictionary@ get_meshUuids()", AngelScript::asMETHOD(SceneGraphImportSettings,get_meshUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "dictionary@ get_materialUuids()", AngelScript::asMETHOD(SceneGraphImportSettings,get_materialUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "dictionary@ get_lightUuids()", AngelScript::asMETHOD(SceneGraphImportSettings,get_lightUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "dictionary@ get_nodeUuids()", AngelScript::asMETHOD(SceneGraphImportSettings,get_nodeUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod(name, "dictionary@ get_cameraUuids()", AngelScript::asMETHOD(SceneGraphImportSettings,get_cameraUuids), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
}

SceneGraphImportSettings* SceneGraphImportSettings::create()
{
  return new SceneGraphImportSettings;
}

} // namespace resources
} // namespace scene
} // namespace glrt

