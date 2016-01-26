#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/camera.h>
#include <glrt/scene/coord-frame.h>
#include <glrt/toolkit/assimp-glm-converter.h>
#include <glrt/toolkit/escape-string.h>

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

inline CoordFrame assimp_global_transformation(const aiNode* node, const aiScene* scene)
{
  CoordFrame p;
  if(node->mParent != nullptr && node->mParent != scene->mRootNode)
    p = assimp_global_transformation(node->mParent, scene);

  return p * CoordFrame(node->mTransformation);
}

void convertStaticMesh_assimpToMesh(const QFileInfo& meshFile, const QFileInfo& sourceFile, bool indexed);
void convertSceneGraph_assimpToSceneGraph(const QFileInfo& meshFile, const QFileInfo& sourceFile, const Uuid<ResourceIndex>& resourceIndexUuid, const SceneGraphImportSettings& settings);

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

void convertSceneGraph_BlenderToCollada(const QFileInfo& sceneGraphFile, const QFileInfo& blenderFile, const Uuid<ResourceIndex>& resourceIndexUuid, const SceneGraphImportSettings &settings)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to scene-graph");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported-scene.dae");

  runBlenderWithPythonScript(python_exportSceneAsColladaSceneGraph(tempFilePath), blenderFile);

  convertSceneGraph_assimpToSceneGraph(sceneGraphFile, tempFilePath, resourceIndexUuid, settings);
}

void convertStaticMesh(const std::string& meshFilename, const std::string& sourceFilename, ResourceIndex*)
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

void convertSceneGraph(const QString& sceneGraphFilename, const QString& sourceFilename, const Uuid<ResourceIndex>& uuid, const SceneGraphImportSettings &settings)
{
  QFileInfo sceneGraphFile(sceneGraphFilename);
  QFileInfo sourceFile(sourceFilename);

  SPLASHSCREEN_MESSAGE(QString("Import scene graph <%0>").arg(sourceFile.fileName()));

  if(shouldConvert(sceneGraphFile, sourceFile))
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


typedef StaticMesh::index_type index_type;
typedef StaticMesh::Vertex Vertex;

StaticMesh loadMeshFromAssimp(aiMesh** meshes, quint32 nMeshes, const glm::mat3& transformation, const QString& context, bool indexed);
void writeToScriptLoadingStaticMesh(QTextStream& stream, const QString& uuid, const StaticMesh& data, int i)
{
  static_assert(sizeof(index_type)==2, "For uint16 as array type to be correct, index_type must be also a 16bit integer");
  stream << "  ";
  if(data.isIndexed())
  {
    stream << "array<uint16> indices";
    if(i>=0)
      stream << i;
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
  }

  stream << "  ";
  stream << "array<float> vertexData";
  if(i>=0)
    stream << i;
  stream << " = \n  {";
  int j=0;
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
  stream << "  loader.loadStaticMesh("<<uuid<<", ";
  if(i>=0)
    stream << "indices"<<i<<", vertexData"<<i<<");\n";
  else
    stream << "indices, vertexData);\n";
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

  StaticMesh data = loadMeshFromAssimp(scene->mMeshes, scene->mNumMeshes, transform, QString("\n(in file <%0>)").arg(sourceFilepath), indexed);

  QFile file(meshFile.absoluteFilePath());

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(meshFile.absoluteFilePath()));

  QTextStream outputStream(&file);

  outputStream << "void main(StaticMeshLoader@ loader, Uuid<StaticMesh> &in uuid)\n{\n";
  writeToScriptLoadingStaticMesh(outputStream, "uuid", data, -1);
  outputStream << "}";
}

struct SceneGraphImportAssets
{
  QVector<Uuid<Material>> materials;

  QHash<QString, Uuid<Camera>> cameraUuids;
  QHash<QString, Camera> cameras;

  QVector<StaticMesh> meshData;
  QHash<QString, QSet<quint32>> meshInstances;
  QVector<Uuid<StaticMesh>> meshes;

  QHash<QString, Uuid<LightSource>> lightUuids;

  QHash<QString, Uuid<Node>> nodeUuids;
  QHash<QString, aiNode*> nodes;

  QHash<QUuid, QString> labels;
  bool indexed = true;
};

void convertSceneGraph_assimpToSceneGraph(const QFileInfo& sceneGraphFile, const QFileInfo& sourceFile, const Uuid<ResourceIndex>& resourceIndexUuid, const SceneGraphImportSettings &settings)
{
  bool fallbackMaterialIsUsed = false;
  bool fallbackLightIsUsed = false;

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

   // use a QMap to sort the meshes by uuid to get a better consistency when working with git
  QMap<QUuid, aiNode*> allNodesToImport;
  QMap<QUuid, uint32_t> allMeshesToImport;

  assets.materials.resize(scene->mNumMaterials);
  for(quint32 i=0; i<scene->mNumMaterials; ++i)
  {
    aiString name;
    if(scene->mMaterials[i]->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
    {
      QString n = name.C_Str();
      n.remove(QRegularExpression("-material$"));
      if(n.isEmpty())
        throw GLRT_EXCEPTION("materials must have a name");

      Uuid<Material> materialUuid;

      if(settings.materialUuids.contains(n))
      {
        materialUuid = settings.materialUuids[n];
      }else
      {
        materialUuid = uuids::fallbackMaterial;
        fallbackMaterialIsUsed = true;
        qWarning() << "NOT DEFINED MATERIAL!\n    Error converting" << sceneGraphFile.filePath() << "to" << sceneGraphFile.filePath() << ":\n    The material"<<n<<"is not provided, using the fallback material instead!!";
      }
      assets.materials[i] = materialUuid;
    }
  }

  for(quint32 i=0; i<scene->mNumCameras; ++i)
  {
    QString n = scene->mCameras[i]->mName.C_Str();
    if(n.isEmpty())
      throw GLRT_EXCEPTION("cameras must have a name");

    if(!settings.shouldImportCamera(n))
      continue;

    assets.cameras[n] = Camera::fromAssimp(*scene->mCameras[i]);

    Uuid<Camera> cameraUuid(QUuid::createUuidV5(QUuid::createUuidV5(resourceIndexUuid, QString("camera[%0]").arg(i)), n));

    if(settings.cameraUuids.contains(n))
      cameraUuid = settings.cameraUuids[n];

    assets.cameraUuids[n] = cameraUuid;
    assets.labels[cameraUuid] = n;
  }

  assets.meshes.resize(scene->mNumMeshes);
  assets.meshData.resize(scene->mNumMeshes);
  for(quint32 i=0; i<scene->mNumMeshes; ++i)
  {
    QString n = scene->mMeshes[i]->mName.C_Str();
    if(n.isEmpty())
      throw GLRT_EXCEPTION("meshes must have a name");

    StaticMesh data = loadMeshFromAssimp(scene->mMeshes+i, 1, glm::mat3(1), QString(" while converting %0 to %1 (occured on mesh %2 (assimp index %3))").arg(sceneGraphFile.filePath()).arg(sceneGraphFile.fileName()).arg(n).arg(i), assets.indexed);;;

    // Is the same instance already used (with a different material?)
    quint32 useIndex = i;
    for(quint32 j : assets.meshInstances[n])
    {
      if(assets.meshData[j] == data)
      {
        useIndex = j;
        data = assets.meshData[j];
      }
    }

    assets.meshData[i] = data;
    assets.meshInstances[n].insert(useIndex);

    Uuid<StaticMesh> meshUuid(QUuid::createUuidV5(QUuid::createUuidV5(resourceIndexUuid, QString("static-mesh[%0]").arg(useIndex)), n));

    if(settings.meshUuids.contains(n))
    {
      if(assets.meshInstances.size() > 1)
        throw GLRT_EXCEPTION(QString("Can't assign a used defined meshUuid (%0) to mesh, where the same name (%1) is used for multiple mesh instances!").arg(QUuid(settings.meshUuids[n]).toString()).arg(n));
      meshUuid = settings.meshUuids[n];
    }

    // Is this the second instance of the mesh just with a different material? => use the same uuid!!
    if(useIndex < i)
    {
      meshUuid = assets.meshes[useIndex];
    }else
    {
      // This is the first instance of this mesh? Also one that should be imported? => import it!
      if(settings.shouldImportMesh(n))
        allMeshesToImport[meshUuid] = i;
    }

    assets.meshes[i] = meshUuid;
    assets.labels[meshUuid] = n;
  }

  for(quint32 i=0; i<scene->mNumLights; ++i)
  {
    QString n = scene->mLights[i]->mName.C_Str();
    if(n.isEmpty())
      throw GLRT_EXCEPTION("lights must have a name");

    Uuid<LightSource> lightUuid;


    if(settings.lightUuids.contains(n))
      lightUuid = settings.lightUuids[n];
    else
    {
      lightUuid = uuids::fallbackLight;
      qWarning() << "NOT DEFINED LIGHT!\n    Error converting" << sceneGraphFile.filePath() << "to" << sceneGraphFile.filePath() << ":\n    The light"<<n<<"is not provided, using the fallback light instead!!";
      fallbackLightIsUsed = true;
    }

    assets.lightUuids[n] = lightUuid;

    assets.labels[lightUuid] = n;
  }

  QStack<aiNode*> nodes;
  nodes.push(scene->mRootNode);
  while(!nodes.isEmpty())
  {
    aiNode* node = nodes.pop();

    for(quint32 i=0; i<node->mNumChildren; ++i)
      nodes.push(node->mChildren[i]);

    QString n = node->mName.C_Str();

    Uuid<Node> nodeUuid = Uuid<Node>(QUuid::createUuidV5(QUuid::createUuidV5(resourceIndexUuid, QString("node")), n));

    if(settings.nodeUuids.contains(n))
      nodeUuid = settings.nodeUuids[n];

    assets.nodeUuids[n] = nodeUuid;

    assets.labels[nodeUuid] = n;
    assets.nodes[n] = node;
    if(settings.shouldImportNode(n))
      allNodesToImport[nodeUuid] = node;
  }

  QFileInfo meshesFile(sceneGraphFile.filePath()+".mesh");

  QFile file(sceneGraphFile.absoluteFilePath());

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(sceneGraphFile.absoluteFilePath()));

  QTextStream outputStream(&file);


  if(!allMeshesToImport.isEmpty())
  {
    QFile meshFile(meshesFile.absoluteFilePath());
    if(!meshFile.open(QFile::WriteOnly))
      throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(meshesFile.absoluteFilePath()));
    QTextStream meshOutputStream(&meshFile);
    meshOutputStream << "void loadMeshes(StaticMeshLoader@ loader)\n{\n";
    for(uint32_t i : allMeshesToImport.values())
    {
      if(i>0)
        meshOutputStream << "\n";
      meshOutputStream << "  // " << scene->mMeshes[i]->mName.C_Str() << "  -- (assimp index: " << i << ")" << "\n";
      writeToScriptLoadingStaticMesh(meshOutputStream, QString("Uuid<StaticMesh>(\"%0\")").arg(QUuid(assets.meshes[i]).toString()), assets.meshData[i], i);
    }
    meshOutputStream << "}";

    outputStream << "#include \"" << escape_angelscript_string(meshesFile.fileName()) << "\"\n";
    outputStream << "\n";
  }

  outputStream << "void main(SceneLayer@ sceneLayer)\n{\n";
  outputStream << "\n";
  outputStream << "  ResourceManager@ resourceManager = sceneLayer.scene.resourceManager;\n";
  outputStream << "\n";
  outputStream << "  Node@ node;\n";
  outputStream << "  Uuid<Node> nodeUuid;\n";
  outputStream << "  Uuid<LightSource> lightUuid;\n";
  outputStream << "  Uuid<StaticMesh> meshUuid;\n";
  outputStream << "  Uuid<CameraComponent> cameraComponentUuid;\n";
  outputStream << "  Uuid<Material> materialUuid;\n";
  if(fallbackLightIsUsed)
    outputStream << "  Uuid<LightSource> fallbackLight = \""<<QUuid(uuids::fallbackLight).toString()<<"\";\n";
  if(fallbackMaterialIsUsed)
    outputStream << "  Uuid<Material> fallbackMaterial = \""<<QUuid(uuids::fallbackMaterial).toString()<<"\";\n";

  if(!allMeshesToImport.isEmpty())
  {
    outputStream << "\n";
    outputStream << "  loadMeshes(resourceManager.staticMeshLoader);\n\n";
  }

  for(aiNode* assimp_node : allNodesToImport.values())
  {
    QString n = assimp_node->mName.C_Str();

    Uuid<Node> nodeUuid = assets.nodeUuids[n];

    bool isUsingMesh = assimp_node->mNumMeshes > 0;
    bool isUsingCamera = assets.cameras.contains(n);
    bool isUsingLight = assets.lightUuids.contains(n);
    bool isUsingComponent = isUsingMesh || isUsingCamera || isUsingLight;
    bool isImportingNode = true;

    if(isImportingNode)
    {
      outputStream << "\n";
      outputStream << "// ======== Node \"" << n << "\" ========\n";
      outputStream << "  nodeUuid = Uuid<Node>(\"" << QUuid(nodeUuid).toString() << "\");\n";
      if(assets.labels.contains(nodeUuid))
        outputStream << "  sceneLayer.index.label[nodeUuid] = \"" << escape_angelscript_string(assets.labels[nodeUuid]) << "\";\n";
      outputStream << "  @node = sceneLayer.newNode(uuid: nodeUuid);\n";

      if(isUsingMesh)
      {
        QMap<QUuid, int> meshesOfNode;
        for(size_t i=0; i<assimp_node->mNumMeshes; ++i)
        {
          int globalMeshIndex = assimp_node->mMeshes[i];
          Uuid<StaticMesh> meshUuid = assets.meshes[globalMeshIndex];
          meshesOfNode[meshUuid] = globalMeshIndex;
        }

        for(int i : meshesOfNode.values())
        {
          aiMesh* mesh = scene->mMeshes[i];
          Uuid<StaticMesh> meshUuid = assets.meshes[i];
          Uuid<Material> materialUuid = assets.materials[mesh->mMaterialIndex];
          outputStream << "  // StaticMesh \""<<mesh->mName.C_Str()<<"\" -- (assimp index "<<i<<")\n";
          outputStream << "  meshUuid = Uuid<StaticMesh>(\"" << QUuid(meshUuid).toString() << "\");\n";
          if(materialUuid == uuids::fallbackMaterial)
            outputStream << "  materialUuid = fallbackMaterial;\n";
          else
            outputStream << "  materialUuid = Uuid<Material>(\"" << QUuid(materialUuid).toString() << "\");\n";
          if(assets.labels.contains(meshUuid))
            outputStream << "  sceneLayer.index.label[meshUuid] = \"" << escape_angelscript_string(assets.labels[meshUuid]) << "\";\n";
          outputStream << "  new_StaticMeshComponent(node: @node, uuid: Uuid<StaticMeshComponent>(\"" << QUuid::createUuidV5(QUuid::createUuidV5(nodeUuid, QUuid(meshUuid).toString()), QString("StaticMeshComponent[%0]").arg(i)).toString() << "\"), "
                       << "isMovable: false, meshUuid: meshUuid, materialUuid: materialUuid);\n";
        }
      }
      if(isUsingCamera)
      {
        const Camera& camera = assets.cameras[n];
        Uuid<Camera> cameraUuid = assets.cameraUuids[n];
        outputStream << "  // Camera \"" << n << "\"\n";
        outputStream << "  cameraComponentUuid = Uuid<CameraComponent>(\"" << QUuid::createUuidV5(QUuid::createUuidV5(nodeUuid, QUuid(cameraUuid).toString()), QString("CameraComponent[%0]").arg(0)).toString() << "\");\n";
        if(assets.labels.contains(cameraUuid))
          outputStream << "  sceneLayer.index.label[cameraComponentUuid] = \"" << escape_angelscript_string(assets.labels[cameraUuid]) << "\";\n";
        outputStream << "  new_CameraComponent(node: @node, uuid: cameraComponentUuid, "
                     << "isMovable: false, aspect: " << camera.aspect << ", clipFar: " << camera.clipFar << ", clipNear: " << camera.clipNear << ", horizontal_fov: " << camera.horizontal_fov << ", lookAt: " << format_angelscript_vec3(camera.lookAt) << ", upVector: " << format_angelscript_vec3(camera.upVector) << ", position: " << format_angelscript_vec3(camera.position) << ");\n";
      }
      if(isUsingLight)
      {
        Uuid<LightSource> lightUuid = assets.lightUuids[n];
        outputStream << "  // Light \"" << n << "\"\n";
        if(lightUuid == uuids::fallbackLight)
          outputStream << "  lightUuid = fallbackLight;\n";
        else
          outputStream << "  lightUuid = Uuid<LightSource>(\"" << QUuid(lightUuid).toString() << "\");\n";
        if(assets.labels.contains(lightUuid))
          outputStream << "  sceneLayer.index.label[lightUuid] = \"" << escape_angelscript_string(assets.labels[lightUuid]) << "\";\n";
        outputStream << "  new_LightComponent(node: @node, uuid: Uuid<LightComponent>(\"" << QUuid::createUuidV5(QUuid::createUuidV5(nodeUuid, QUuid(lightUuid).toString()), QString("LightComponent[%0]").arg(0)).toString() << "\"), "
                     << "lightSourceUuid: lightUuid, interactivity: LightSourceInteractivity::STATIC);\n";
      }
      if(!isUsingComponent)
      {
        outputStream << "  new_EmptyComponent(node: @node, uuid: Uuid<NodeComponent>(\"" << QUuid::createUuidV5(nodeUuid, QString("missing-root-node-for-transformation")).toString() << "\"), isMovable: false);\n";
        isUsingComponent = true;
      }

      if(isUsingComponent)
      {
        CoordFrame frame(assimp_global_transformation(assimp_node, scene));
        outputStream << "  node.rootComponent.localTransformation = "<< frame.as_angelscript_fast() <<";\n";
      }
    }
  }
  outputStream << "}";
}

StaticMesh loadMeshFromAssimp(aiMesh** meshes, quint32 nMeshes, const glm::mat3& transform, const QString& context, bool indexed)
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

  return StaticMesh{indices, vertices};
}



struct SceneGraphImportSettings::AngelScriptInterface final : public AngelScriptIntegration::RefCountedObject
{
  friend class SceneGraphImportSettings;

  AngelScript::CScriptArray* as_meshesToImport;
  AngelScript::CScriptArray* as_camerasToImport;
  AngelScript::CScriptArray* as_nodesToImport;

  AngelScript::CScriptDictionary* as_meshUuids;
  AngelScript::CScriptDictionary* as_materialUuids;
  AngelScript::CScriptDictionary* as_lightUuids;
  AngelScript::CScriptDictionary* as_nodeUuids;
  AngelScript::CScriptDictionary* as_cameraUuids;
  static void registerType();

  AngelScriptInterface();
  ~AngelScriptInterface();

  AngelScriptInterface(const SceneGraphImportSettings&) = delete;
  AngelScriptInterface(SceneGraphImportSettings&&) = delete;
  AngelScriptInterface&operator=(const SceneGraphImportSettings&) = delete;
  AngelScriptInterface&operator=(SceneGraphImportSettings&&) = delete;

  static AngelScriptInterface* create();
  static bool shouldImport(const QString& name, const QSet<QString>& patternsToImport);
};


SceneGraphImportSettings::AngelScriptInterface::AngelScriptInterface()
{
  int meshUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<StaticMesh>");
  int materialUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Material>");
  int lightUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<LightSource>");
  int nodeUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Node>");
  int cameraUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Camera>");

  as_meshesToImport = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);
  as_camerasToImport = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);
  as_nodesToImport = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);

  as_meshUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<StaticMesh>>(), meshUuidTypeId, angelScriptEngine);
  as_materialUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<Material>>(), materialUuidTypeId, angelScriptEngine);
  as_lightUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<LightSource>>(), lightUuidTypeId, angelScriptEngine);
  as_nodeUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<Node>>(), nodeUuidTypeId, angelScriptEngine);
  as_cameraUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<Camera>>(), cameraUuidTypeId, angelScriptEngine);
}

SceneGraphImportSettings::AngelScriptInterface::~AngelScriptInterface()
{
  as_meshesToImport->Release();
  as_camerasToImport->Release();
  as_nodesToImport->Release();

  as_meshUuids->Release();
  as_materialUuids->Release();
  as_lightUuids->Release();
  as_nodeUuids->Release();
  as_cameraUuids->Release();
}

SceneGraphImportSettings::AngelScriptInterface* SceneGraphImportSettings::AngelScriptInterface::create()
{
  return new AngelScriptInterface;
}

bool SceneGraphImportSettings::shouldImportMesh(const QString& name) const
{
  return shouldImport(name, meshesToImport) || shouldImport(name+"-mesh", meshesToImport);
}

bool SceneGraphImportSettings::shouldImportCamera(const QString& name) const
{
  return shouldImport(name, camerasToImport) || shouldImport(name+"-camera", camerasToImport);
}

bool SceneGraphImportSettings::shouldImportNode(const QString& name) const
{
  return shouldImport(name, nodesToImport) || shouldImport(name+"-node", nodesToImport);
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

SceneGraphImportSettings::SceneGraphImportSettings(AngelScriptInterface* interface)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("BaseUuid");
  int staticMeshUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<StaticMesh>");
  int materialUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Material>");
  int lightUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<LightSource>");
  int nodeUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Node>");
  int cameraUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Camera>");

  meshesToImport = AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToImport);
  camerasToImport = AngelScriptIntegration::scriptArrayToStringSet(interface->as_camerasToImport);
  nodesToImport = AngelScriptIntegration::scriptArrayToStringSet(interface->as_nodesToImport);

  meshUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<StaticMesh>>(interface->as_meshUuids, {uuidTypeId, staticMeshUuidTypeId});
  materialUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<Material>>(interface->as_materialUuids, {uuidTypeId, materialUuidTypeId});
  lightUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<LightSource>>(interface->as_lightUuids, {uuidTypeId, lightUuidTypeId});
  nodeUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<Node>>(interface->as_nodeUuids, {uuidTypeId, nodeUuidTypeId});
  cameraUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<Camera>>(interface->as_cameraUuids, {uuidTypeId, cameraUuidTypeId});

  interface->releaseReference();
}

void SceneGraphImportSettings::registerType()
{
  int r;
  const char* name = "SceneGraphImportSettings";

  r = angelScriptEngine->RegisterObjectType(name, 0, AngelScript::asOBJ_REF); AngelScriptCheck(r);

  AngelScriptIntegration::RefCountedObject::registerAsBaseOfClass<SceneGraphImportSettings::AngelScriptInterface>(angelScriptEngine, name);

  r = angelScriptEngine->RegisterObjectBehaviour(name, AngelScript::asBEHAVE_FACTORY, (std::string(name)+"@ f()").c_str(), AngelScript::asFUNCTION(SceneGraphImportSettings::AngelScriptInterface::create), AngelScript::asCALL_CDECL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesToImport", asOFFSET(AngelScriptInterface, as_meshesToImport)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ camerasToImport", asOFFSET(AngelScriptInterface,as_camerasToImport)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ nodesToImport", asOFFSET(AngelScriptInterface,as_nodesToImport)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ meshUuids", asOFFSET(AngelScriptInterface,as_meshUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ materialUuids", asOFFSET(AngelScriptInterface,as_materialUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ lightUuids", asOFFSET(AngelScriptInterface,as_lightUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ nodeUuids", asOFFSET(AngelScriptInterface,as_nodeUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ cameraUuids", asOFFSET(AngelScriptInterface,as_cameraUuids)); AngelScriptCheck(r);
}

} // namespace resources
} // namespace scene
} // namespace glrt

