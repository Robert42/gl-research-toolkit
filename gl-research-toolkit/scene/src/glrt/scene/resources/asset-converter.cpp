#include <glrt/scene/resources/asset-converter.h>
#include <glrt/scene/resources/static-mesh.h>
#include <glrt/scene/resources/resource-index.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/static-mesh-file.h>
#include <glrt/scene/camera-parameter.h>
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
QString python_exportSceneAsObjMesh(const QString& objFile, const QString& groupToImport);
QString python_exportSceneAsColladaSceneGraph(const QString& objFile, const QString& groupToImport);

bool hasMatch(const QString& name, const QString& pattern);
bool hasMatch(const QString& name, const QSet<QString>& patternsToImport);
bool hasMatch(const aiString& name, const QSet<QString>& patternsToImport);

void convertStaticMesh_BlenderToObj(const QFileInfo& meshFile, const QFileInfo& blenderFile, const QString& groupToImport, bool indexed)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to static mesh");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported.obj");

  runBlenderWithPythonScript(python_exportSceneAsObjMesh(tempFilePath, groupToImport), blenderFile);

  convertStaticMesh_assimpToMesh(meshFile, tempFilePath, indexed);
}

void convertSceneGraph_BlenderToCollada(const QFileInfo& sceneGraphFile, const QFileInfo& blenderFile, const Uuid<ResourceIndex>& resourceIndexUuid, const SceneGraphImportSettings &settings, const QString& groupToImport)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to scene-graph");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported-scene.dae");

  runBlenderWithPythonScript(python_exportSceneAsColladaSceneGraph(tempFilePath, groupToImport), blenderFile);

  convertSceneGraph_assimpToSceneGraph(sceneGraphFile, tempFilePath, resourceIndexUuid, settings);
}

void convertStaticMesh(const QString& meshFilename, const QString& sourceFilename, const QString& groupToImport, const MeshImportSettings& meshImportSettings)
{
  bool indexed = meshImportSettings.indexed;

  QFileInfo meshFile(meshFilename);
  QFileInfo sourceFile(sourceFilename);

  if(SHOULD_CONVERT(meshFile, sourceFile))
  {
    SPLASHSCREEN_MESSAGE(QString("Import static mesh <%0>").arg(sourceFile.fileName()));
    if(sourceFile.suffix().toLower() == "blend")
      convertStaticMesh_BlenderToObj(meshFile, sourceFile, groupToImport, indexed);
    else
      convertStaticMesh_assimpToMesh(meshFile, sourceFile, indexed);
  }
}

void convertSceneGraph(const QString& sceneGraphFilename, const QString& sourceFilename, const Uuid<ResourceIndex>& uuid, const SceneGraphImportSettings &settings, const QString& groupToImport)
{
  QFileInfo sceneGraphFile(sceneGraphFilename);
  QFileInfo sourceFile(sourceFilename);

  if(SHOULD_CONVERT(sceneGraphFile, sourceFile))
  {
    SPLASHSCREEN_MESSAGE(QString("Import scene graph <%0>").arg(sourceFile.fileName()));
    qDebug() << "convertSceneGraph("<<sceneGraphFile.fileName()<<", "<<sourceFile.fileName()<<", group: " << groupToImport << ")";
    convertSceneGraph_BlenderToCollada(sceneGraphFile, sourceFile, uuid, settings, groupToImport);
  }
}

void convertTexture(const QString& textureFilename, const QString& sourceFilename, const TextureFile::ImportSettings& textureImportSettings)
{
  QFileInfo textureFile(textureFilename);
  QFileInfo sourceFile(sourceFilename);

  if(SHOULD_CONVERT(textureFilename, sourceFile))
  {
    SPLASHSCREEN_MESSAGE(QString("Import texture <%0>").arg(sourceFile.fileName()));
    qDebug() << "convertTexture("<<textureFile.fileName()<<", "<<sourceFile.fileName()<<")";

    TextureFile texture;
    texture.import(sourceFile, textureImportSettings);
    texture.save(textureFile);
  }
}

void runBlenderWithPythonScript(const QString& pythonScript, const QFileInfo& blenderFile)
{
  // Hack to find the newest blender version on the PC in my campus
  QString blenderProgram("blender-newest");
  if(QProcess::execute(blenderProgram, {"--version"}) !=0)
    blenderProgram = "blender";

  QStringList arguments = {"--background", blenderFile.absoluteFilePath(), "--python-expr", pythonScript};

  qDebug() << "Opening the Blender scene '"<<blenderFile.absoluteFilePath()<<"' and executing the following script:";
  qDebug() << pythonScript.toStdString().c_str();

  if(QProcess::execute(blenderProgram, arguments) !=0)
    throw GLRT_EXCEPTION("Executing Blender failed");
}

QString to_python_string(QString str)
{
  return str.replace("\\", R"("'\\'")").replace("\"", "\\\"").prepend("r\"").append("\"");
}

QString python_select_group_only(const QString& groupToImport)
{
  QString pythonScript;

  if(!groupToImport.isEmpty())
  {
    pythonScript += "# The selection is used to control, which objects are to be exported.\n";
    pythonScript += "# To controll the selection of objects, we need to be in the object mode\n";
    pythonScript += "if bpy.context.scene.objects.active is not None:\n";
    pythonScript += "    bpy.ops.object.mode_set(mode='OBJECT')\n";
    pythonScript += "# all selected objects are exported. So make sure no objects outside of the group are selected\n";
    pythonScript += "bpy.ops.object.select_all(action='DESELECT')\n";
    pythonScript += QString("groupName = '%0'\n").arg(groupToImport);
    pythonScript += "if not groupName in bpy.data.groups:\n";
    pythonScript += "    raise ValueError(\"ERROR: couldn't find the group {0}\".format(groupName))\n";
    pythonScript += "group = bpy.data.groups[groupName]\n";
    pythonScript += "for object in group.objects:\n";
    pythonScript += "    # only visible objects are exported. So make sure all objects of the group are visible\n";
    pythonScript += "    object.hide = False\n"; // This should be optional in a future implementation
    pythonScript += "    # only selected objects are exported. So make sure all objects of the group are selected\n";
    pythonScript += "    object.select = True\n";
    pythonScript += "    # Only objects in visible layers are exported so make sure that all layers containing objects of the group are visible.\n";
    pythonScript += "    for i in range(0, 20):\n";
    pythonScript += "        if object.layers[i]:\n";
    pythonScript += "            bpy.context.scene.layers[i] = True\n";
  }

  return pythonScript;
}

QString python_exportSceneAsObjMesh(const QString& objFile, const QString& groupToImport)
{
  QString pythonScript = "import bpy\n";

  pythonScript += python_select_group_only(groupToImport);

  // Missing: automatically switch to object mode?

  pythonScript += QString("bpy.ops.export_scene.obj("
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
                          "axis_up='Z'").arg(to_python_string(objFile));

  if(!groupToImport.isEmpty())
    pythonScript += ", use_selection=True";

  pythonScript += ")";

  return pythonScript;
}

QString python_exportSceneAsColladaSceneGraph(const QString& colladaFile, const QString& groupToImport)
{
  QString pythonScript = "import bpy\n";

  pythonScript += python_select_group_only(groupToImport);

  pythonScript += QString("import bpy\n"
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
                          ).arg(to_python_string(colladaFile));

  if(!groupToImport.isEmpty())
    pythonScript += ", selected=True";

  pythonScript += ")";

  return pythonScript;
}


typedef StaticMesh::index_type index_type;
typedef StaticMesh::Vertex Vertex;

StaticMesh loadMeshFromAssimp(aiMesh** meshes, quint32 nMeshes, const glm::mat3& transformation, const QString& context, bool indexed);

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

  StaticMeshFile staticMeshFile;
  staticMeshFile.staticMesh = data;
  staticMeshFile.save(meshFile);
}

struct SceneGraphImportAssets
{
  QVector<Uuid<Material>> materials;

  QHash<QString, Uuid<CameraParameter>> cameraUuids;
  QHash<QString, CameraParameter> cameras;

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
  qDebug() << "convertSceneGraph_assimpToSceneGraph(" << sceneGraphFile.absoluteFilePath() << "," << sourceFile.absoluteFilePath() << ")";

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
  QSet<Uuid<StaticMesh>> meshesToVoxelize;
  QSet<Uuid<StaticMesh>> meshesToVoxelizeJoined;
  QSet<Uuid<StaticMesh>> meshesToVoxelizeWithManifold;
  QSet<Uuid<StaticMesh>> twoSidedMeshes;
  QSet<Uuid<StaticMesh>> singleSidedMeshes;
  QList<QSet<Uuid<StaticMesh>>> meshesToJoin;
  QHash<Uuid<StaticMesh>, QVector<CoordFrame>> meshesToJoin_Transformations;
  QSet<QSet<Uuid<StaticMesh>>> instancedMeshesToJoin;

  QHash<Uuid<StaticMesh>, float> meshVoxelizeScaleFactors;

  for(int i=0; i<settings.meshesToMergeWhenVoxelizing.length(); ++i)
    meshesToJoin.append(QSet<Uuid<StaticMesh>>());

  QUuid baseUuid = QUuid::createUuidV5(QUuid::createUuidV5(resourceIndexUuid, sceneGraphFile.fileName()), sourceFile.fileName());

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

    assets.cameras[n] = CameraParameter::fromAssimp(*scene->mCameras[i]);

    Uuid<CameraParameter> cameraUuid(QUuid::createUuidV5(QUuid::createUuidV5(baseUuid, QString("camera[%0]").arg(i)), n));

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
      continue;

    Q_ASSERT(quint32(std::numeric_limits<int>::max()) >= i);

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

    Uuid<StaticMesh> meshUuid(QUuid::createUuidV5(QUuid::createUuidV5(baseUuid, QString("static-mesh[%0]").arg(useIndex)), n));

    if(settings.meshUuids.contains(n))
    {
      if(assets.meshInstances[n].size() > 1)
        throw GLRT_EXCEPTION(QString("Can't assign a used defined meshUuid (%0) to mesh, where the same name (%1) is used for multiple mesh instances!").arg(QUuid(settings.meshUuids[n]).toString()).arg(n));
      meshUuid = settings.meshUuids[n];
    }else
    {
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
    }


    for(int i=0; i<settings.meshesToMergeWhenVoxelizing.length(); ++i)
    {
      const QSet<QString>& set = settings.meshesToMergeWhenVoxelizing[i];
      if(hasMatch(n, set))
      {
        meshesToJoin[i].insert(meshUuid);
        meshesToVoxelizeJoined.insert(meshUuid);
      }
    }

    meshVoxelizeScaleFactors[meshUuid] = settings.meshVoxelizeScaleFactorForMeshName(n);

    assets.meshes[i] = meshUuid;
    assets.labels[meshUuid] = n;
  }

  for(const QSet<QString>& set : settings.meshesToMergeWhenVoxelizingInstanced)
  {
    QSet<Uuid<StaticMesh>> _meshUuidSet;

    for(const QString& n : assets.meshInstances.keys())
    {
      if(hasMatch(n, set))
      {
        for(int i : assets.meshInstances.value(n))
        {
          Uuid<StaticMesh> meshUuid = assets.meshes[i];
          _meshUuidSet.insert(meshUuid);
          meshesToVoxelizeJoined.insert(meshUuid);
        }
      }
    }

    if(!_meshUuidSet.isEmpty())
    {
      instancedMeshesToJoin.insert(_meshUuidSet);
    }
  }

  // If Assimp imports multiple meshes with the same name, this means, it has split them because of one mesh using multiple materials.
  // This has no relevance for voxelizing, so mark meshes with multiple materials into the same set within meshesToJoin
  for(const QString& n : assets.meshInstances.keys())
  {
    const QSet<quint32>& meshesWithSameName_Indices = assets.meshInstances[n];

    if(meshesWithSameName_Indices.size() < 2)
      continue;

    QSet<Uuid<StaticMesh>> meshesWithSameName;
    for(quint32 i : meshesWithSameName_Indices)
    {
      Uuid<StaticMesh> meshUuid = assets.meshes[i];

      meshesWithSameName.insert(meshUuid);
      meshesToVoxelizeJoined.insert(meshUuid);
    }

    instancedMeshesToJoin.insert(meshesWithSameName);
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

    Uuid<Node> nodeUuid = Uuid<Node>(QUuid::createUuidV5(QUuid::createUuidV5(baseUuid, QString("node")), n));

    if(settings.nodeUuids.contains(n))
      nodeUuid = settings.nodeUuids[n];

    assets.nodeUuids[n] = nodeUuid;

    assets.labels[nodeUuid] = n;
    assets.nodes[n] = node;
    if(settings.shouldImportNode(n))
      allNodesToImport[nodeUuid] = node;
  }

  QFileInfo assetIndexFileInfo(sceneGraphFile.filePath()+".asset-index");

  QFile sceneGraph_file(sceneGraphFile.absoluteFilePath());

  if(!sceneGraph_file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(sceneGraphFile.absoluteFilePath()));

  QTextStream sceneGraph_outputStream(&sceneGraph_file);


  sceneGraph_outputStream << "void main(SceneLayer@ sceneLayer)\n{\n";
  sceneGraph_outputStream << "\n";
  sceneGraph_outputStream << "  Node@ node;\n";
  sceneGraph_outputStream << "  Uuid<Node> nodeUuid;\n";
  sceneGraph_outputStream << "  Uuid<LightSource> lightUuid;\n";
  sceneGraph_outputStream << "  Uuid<StaticMesh> meshUuid;\n";
  sceneGraph_outputStream << "  Uuid<CameraComponent> cameraComponentUuid;\n";
  sceneGraph_outputStream << "  Uuid<Material> materialUuid;\n";
  if(fallbackLightIsUsed)
    sceneGraph_outputStream << "  Uuid<LightSource> fallbackLight = Uuid<LightSource>(\""<<QUuid(uuids::fallbackLight).toString()<<"\");\n";
  if(fallbackMaterialIsUsed)
    sceneGraph_outputStream << "  Uuid<Material> fallbackMaterial = Uuid<Material>(\""<<QUuid(uuids::fallbackMaterial).toString()<<"\");\n";

  for(aiNode* assimp_node : allNodesToImport.values())
  {
    QString n = assimp_node->mName.C_Str();

    Uuid<Node> nodeUuid = assets.nodeUuids[n];

    bool isUsingMesh = assimp_node->mNumMeshes > 0;
    bool isUsingCamera = assets.cameras.contains(n);
    bool isUsingLight = assets.lightUuids.contains(n);
    bool isUsingComponent = false;
    bool isImportingNode = true;

    if(isImportingNode)
    {
      CoordFrame frame(assimp_global_transformation(assimp_node, scene));

      sceneGraph_outputStream << "\n";
      sceneGraph_outputStream << "// ======== Node \"" << n << "\" ========\n";
      sceneGraph_outputStream << "  nodeUuid = Uuid<Node>(\"" << QUuid(nodeUuid).toString() << "\");\n";
      if(assets.labels.contains(nodeUuid))
        sceneGraph_outputStream << "  sceneLayer.index.label[nodeUuid] = \"" << escape_angelscript_string(assets.labels[nodeUuid]) << "\";\n";
      sceneGraph_outputStream << "  @node = sceneLayer.newNode(uuid: nodeUuid);\n";

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
          Uuid<StaticMesh> meshUuid = assets.meshes[i];
          if(meshUuid == Uuid<StaticMesh>())
            continue;
          aiMesh* mesh = scene->mMeshes[i];
          Uuid<Material> materialUuid = assets.materials[mesh->mMaterialIndex];
          if(!ResourceManager::instance()->isRegistered(materialUuid))
            throw GLRT_EXCEPTION(QString("Using not registered material in imported scene-graph-file %0").arg(sceneGraphFile.absoluteFilePath()));
          Material::Type materialType = ResourceManager::instance()->materialForUuid(materialUuid).type;
          if(hasMatch(mesh->mName, settings.meshesToVoxelize) && !hasMatch(mesh->mName, settings.meshesNoToVoxelize))
          {
            meshesToVoxelize.insert(meshUuid);
            if(hasMatch(mesh->mName, settings.meshesToVoxelizeTwoSided))
            {
              twoSidedMeshes.insert(meshUuid);
            }else
            {
              bool twoSided = materialType.testFlag(Material::TypeFlag::TWO_SIDED);
              if(twoSided)
                twoSidedMeshes.insert(meshUuid);
              else
                singleSidedMeshes.insert(meshUuid);
            }

            if(hasMatch(mesh->mName, settings.meshesToVoxelizeWithManifold))
              meshesToVoxelizeWithManifold.insert(meshUuid);


            for(int i=0; i<settings.meshesToMergeWhenVoxelizing.length(); ++i)
            {
              if(meshesToJoin[i].contains(meshUuid))
                meshesToJoin_Transformations[meshUuid].append(frame);
            }
          }
          sceneGraph_outputStream << "  // StaticMesh \""<<mesh->mName.C_Str()<<"\" -- (assimp index "<<i<<")\n";
          sceneGraph_outputStream << "  meshUuid = Uuid<StaticMesh>(\"" << QUuid(meshUuid).toString() << "\");\n";
          if(materialUuid == uuids::fallbackMaterial)
            sceneGraph_outputStream << "  materialUuid = fallbackMaterial;\n";
          else
            sceneGraph_outputStream << "  materialUuid = Uuid<Material>(\"" << QUuid(materialUuid).toString() << "\");\n";
          if(assets.labels.contains(meshUuid))
            sceneGraph_outputStream << "  sceneLayer.index.label[meshUuid] = \"" << escape_angelscript_string(assets.labels[meshUuid]) << "\";\n";
          sceneGraph_outputStream << "  new_StaticMeshComponent(node: @node, uuid: Uuid<StaticMeshComponent>(\"" << QUuid::createUuidV5(QUuid::createUuidV5(nodeUuid, QUuid(meshUuid).toString()), QString("StaticMeshComponent[%0]").arg(i)).toString() << "\"), "
                       << "meshUuid: meshUuid, materialUuid: materialUuid);\n";
          isUsingComponent = true;
        }
      }
      if(isUsingCamera)
      {
        const CameraParameter& camera = assets.cameras[n];
        Uuid<CameraParameter> cameraUuid = assets.cameraUuids[n];
        sceneGraph_outputStream << "  // Camera \"" << n << "\"\n";
        sceneGraph_outputStream << "  cameraComponentUuid = Uuid<CameraComponent>(\"" << QUuid::createUuidV5(QUuid::createUuidV5(nodeUuid, QUuid(cameraUuid).toString()), QString("CameraComponent[%0]").arg(0)).toString() << "\");\n";
        if(assets.labels.contains(cameraUuid))
          sceneGraph_outputStream << "  sceneLayer.index.label[cameraComponentUuid] = \"" << escape_angelscript_string(assets.labels[cameraUuid]) << "\";\n";
        sceneGraph_outputStream << "  new_CameraComponent(node: @node, uuid: cameraComponentUuid, "
                     << "aspect: " << camera.aspect << ", clipFar: " << camera.clipFar << ", clipNear: " << camera.clipNear << ", horizontal_fov: " << camera.horizontal_fov << ", lookAt: " << format_angelscript_vec3(camera.lookAt) << ", upVector: " << format_angelscript_vec3(camera.upVector) << ", position: " << format_angelscript_vec3(camera.position) << ");\n";
        isUsingComponent = true;
      }
      if(isUsingLight)
      {
        Uuid<LightSource> lightUuid = assets.lightUuids[n];
        sceneGraph_outputStream << "  // Light \"" << n << "\"\n";
        if(lightUuid == uuids::fallbackLight)
          sceneGraph_outputStream << "  lightUuid = fallbackLight;\n";
        else
          sceneGraph_outputStream << "  lightUuid = Uuid<LightSource>(\"" << QUuid(lightUuid).toString() << "\");\n";
        if(assets.labels.contains(lightUuid))
          sceneGraph_outputStream << "  sceneLayer.index.label[lightUuid] = \"" << escape_angelscript_string(assets.labels[lightUuid]) << "\";\n";
        sceneGraph_outputStream << "  new_LightComponent(node: @node, uuid: Uuid<LightComponent>(\"" << QUuid::createUuidV5(QUuid::createUuidV5(nodeUuid, QUuid(lightUuid).toString()), QString("LightComponent[%0]").arg(0)).toString() << "\"), "
                     << "lightSourceUuid: lightUuid);\n";
        isUsingComponent = true;
      }
      if(!isUsingComponent)
      {
        sceneGraph_outputStream << "  new_EmptyComponent(node: @node, uuid: Uuid<NodeComponent>(\"" << QUuid::createUuidV5(nodeUuid, QString("missing-root-node-for-transformation")).toString() << "\"));\n";
        isUsingComponent = true;
      }

      if(isUsingComponent)
      {
        sceneGraph_outputStream << "  node.rootComponent.localTransformation = "<< frame.as_angelscript_fast() <<";\n";
      }
    }
  }
  sceneGraph_outputStream << "}";
  sceneGraph_file.close();

  if(!allMeshesToImport.isEmpty())
  {
    QFile assetIndex_file(assetIndexFileInfo.absoluteFilePath());
    if(!assetIndex_file.open(QFile::WriteOnly))
      throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(assetIndexFileInfo.absoluteFilePath()));

    QTextStream assetIndex_outputStream(&assetIndex_file);
    assetIndex_outputStream << "void main(ResourceIndex@ index)\n{\n";
    sceneGraph_outputStream << "\n";

    assetIndex_outputStream << "  Voxelizer voxelizer = index.sceneVoxelizer;\n";

    float scaleFactor = 1.f;

    for(uint32_t i : allMeshesToImport.values())
    {
      const Uuid<StaticMesh>& uuid = assets.meshes[i];
      const QString label = assets.labels[uuid];
      QString filename = QString("%0%1.mesh").arg(label).arg(uuid.toString());
      filename = QFileInfo(filename).absoluteFilePath();
      assetIndex_outputStream << QString("  index.registerStaticMesh(uuid: Uuid<StaticMesh>(\"%0\"), file: \"%1\");\n").arg(uuid.toString()).arg(filename);
      StaticMeshFile meshFile;
      meshFile.staticMesh = assets.meshData[i];
      meshFile.save(filename);

      if(meshesToVoxelize.contains(uuid) && !meshesToVoxelizeJoined.contains(uuid))
      {
        if(twoSidedMeshes.contains(uuid) && singleSidedMeshes.contains(uuid))
          throw GLRT_EXCEPTION(QString("Couldn't open file <%0> for writing.").arg(sceneGraphFile.absoluteFilePath()));

        Voxelizer::MeshType meshType = twoSidedMeshes.contains(uuid) ? Voxelizer::MeshType::TWO_SIDED : meshesToVoxelizeWithManifold.contains(uuid) ? Voxelizer::MeshType::MANIFOLD_RAY_CHECK : Voxelizer::MeshType::FACE_SIDE;

        const float currentScaleFactor = meshVoxelizeScaleFactors[uuid];
        if(scaleFactor != currentScaleFactor)
        {
          scaleFactor = currentScaleFactor;
          assetIndex_outputStream << QString("  voxelizer.signedDistanceField.scaleFactor = %0;\n").arg(scaleFactor);
        }

        QString proxyMesh;
        if(settings.meshVoxelizeProxies.contains(label))
          proxyMesh = QString(", proxyStaticMesh: Uuid<StaticMesh>(\"%0\")").arg(settings.meshVoxelizeProxies.value(label, uuid).toString());
        QString voxelizationCommand = QString("  voxelizer.voxelize(staticMeshUuid: Uuid<StaticMesh>(\"%0\"), meshType: %1%2);\n").arg(uuid.toString()).arg(Voxelizer::toAngelScript(meshType)).arg(proxyMesh);
        assetIndex_outputStream << voxelizationCommand;
      }
    }

    for(const QSet<Uuid<StaticMesh>>& joinedGroup : meshesToJoin)
    {
      QHash<Uuid<StaticMesh>, QVector<CoordFrame>> transformations;

      for(Uuid<StaticMesh> meshUuid : joinedGroup)
      {
        QVector<CoordFrame> t = meshesToJoin_Transformations.value(meshUuid);

        if(meshesToVoxelize.contains(meshUuid) && !t.isEmpty())
          transformations.insert(meshUuid, t);
      }

      if(transformations.isEmpty())
        continue;

      assetIndex_outputStream << "  voxelizer.beginGroup();\n";

      float scaleFactor = 0.f;
      Voxelizer::MeshType meshType = Voxelizer::MeshType::FACE_SIDE;
      for(auto i = transformations.begin(); i!=transformations.end(); ++i)
      {
        if(meshesToVoxelizeWithManifold.contains(i.key()))
          meshType = Voxelizer::MeshType::MANIFOLD_RAY_CHECK;
        scaleFactor = glm::max<float>(scaleFactor, meshVoxelizeScaleFactors[i.key()]);
        for(const CoordFrame& frame : i.value())
          assetIndex_outputStream << QString("  voxelizer.addToGroup(staticMeshUuid: Uuid<StaticMesh>(\"%0\"), coordFrame: %1%3); // %2\n").arg(i.key().toString()).arg(frame.as_angelscript_fast()).arg(assets.labels.value(i.key())).arg(twoSidedMeshes.contains(i.key()) ? ", two_sided: true" : "");
      }
      assetIndex_outputStream << QString("  voxelizer.signedDistanceField.scaleFactor = %0;\n").arg(scaleFactor);
      assetIndex_outputStream << "  voxelizer.voxelizeGroup(" << Voxelizer::toAngelScript(meshType) << ");\n";
    }

    for(const QSet<Uuid<StaticMesh>>& joinedGroup : instancedMeshesToJoin)
    {
      if(joinedGroup.isEmpty())
        continue;

      assetIndex_outputStream << "  voxelizer.beginGroup();\n";
      Voxelizer::MeshType meshType = Voxelizer::MeshType::FACE_SIDE;
      float scaleFactor = 0.f;
      for(const Uuid<StaticMesh>& staticMeshUuid : joinedGroup)
      {
        if(meshesToVoxelizeWithManifold.contains(staticMeshUuid))
          meshType = Voxelizer::MeshType::MANIFOLD_RAY_CHECK;
        scaleFactor = glm::max<float>(scaleFactor, meshVoxelizeScaleFactors[staticMeshUuid]);
        assetIndex_outputStream << QString("  voxelizer.addToGroup(staticMeshUuid: Uuid<StaticMesh>(\"%0\")%1); // %2\n").arg(staticMeshUuid.toString()).arg(twoSidedMeshes.contains(staticMeshUuid) ? ", two_sided: true" : "").arg(assets.labels.value(staticMeshUuid));
      }
      assetIndex_outputStream << QString("  voxelizer.signedDistanceField.scaleFactor = %0;\n").arg(scaleFactor);
      assetIndex_outputStream << "  voxelizer.voxelizeGroup(" << Voxelizer::toAngelScript(meshType) << ");\n";
    }

    assetIndex_outputStream << "}";
  }else if(assetIndexFileInfo.exists())
  {
    QFile::remove(assetIndexFileInfo.absoluteFilePath());
  }
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
    throw GLRT_EXCEPTION(QString("Too many vertices%0. Got %1, the maximum ist %2").arg(context).arg(numVertices).arg(std::numeric_limits<index_type>::max()));

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
      vertex.bitangent = transform * to_glm_vec3(mesh->mBitangents[j]);
      vertex.uv = to_glm_vec3(mesh->mTextureCoords[0][j]).xy();

      vertex.cleanUp();

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
  AngelScript::CScriptArray* as_meshesToMergeWhenVoxelizing;
  AngelScript::CScriptArray* as_meshesToMergeWhenVoxelizingInstanced;
  AngelScript::CScriptArray* as_meshesToVoxelize;
  AngelScript::CScriptArray* as_meshesNotToVoxelize;
  AngelScript::CScriptArray* as_meshesToVoxelizeTwoSided;
  AngelScript::CScriptArray* as_meshesToVoxelizeWithManifold;

  AngelScript::CScriptDictionary* as_meshVoxelizeProxies;
  AngelScript::CScriptDictionary* as_meshVoxelizeScaleFactors;
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
  int cameraUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<CameraParameter>");
  int floatTypeId = angelScriptEngine->GetTypeIdByDecl("float");

  as_meshesToImport = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);
  as_camerasToImport = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);
  as_nodesToImport = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);
  as_meshesToVoxelize = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({".*"}), angelScriptEngine);
  as_meshesNotToVoxelize = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({}), angelScriptEngine);
  as_meshesToMergeWhenVoxelizing = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({}), angelScriptEngine);
  as_meshesToMergeWhenVoxelizingInstanced = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({}), angelScriptEngine);
  as_meshesToVoxelizeTwoSided = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({}), angelScriptEngine);
  as_meshesToVoxelizeWithManifold = AngelScriptIntegration::scriptArrayFromStringSet(QSet<QString>({}), angelScriptEngine);

  as_meshVoxelizeProxies = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<StaticMesh>>(), meshUuidTypeId, angelScriptEngine);
  as_meshVoxelizeScaleFactors = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<StaticMesh>>(), floatTypeId, angelScriptEngine);
  as_meshUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<StaticMesh>>(), meshUuidTypeId, angelScriptEngine);
  as_materialUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<Material>>(), materialUuidTypeId, angelScriptEngine);
  as_lightUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<LightSource>>(), lightUuidTypeId, angelScriptEngine);
  as_nodeUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<Node>>(), nodeUuidTypeId, angelScriptEngine);
  as_cameraUuids = AngelScriptIntegration::scriptDictionaryFromHash(QHash<QString, Uuid<CameraParameter>>(), cameraUuidTypeId, angelScriptEngine);
}

SceneGraphImportSettings::AngelScriptInterface::~AngelScriptInterface()
{
  as_meshesToImport->Release();
  as_camerasToImport->Release();
  as_nodesToImport->Release();
  as_meshesToVoxelize->Release();
  as_meshesNotToVoxelize->Release();
  as_meshesToMergeWhenVoxelizing->Release();
  as_meshesToMergeWhenVoxelizingInstanced->Release();
  as_meshesToVoxelizeTwoSided->Release();
  as_meshesToVoxelizeWithManifold->Release();

  as_meshVoxelizeProxies->Release();
  as_meshVoxelizeScaleFactors->Release();
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
  return hasMatch(name, meshesToImport) || hasMatch(name+"-mesh", meshesToImport);
}

bool SceneGraphImportSettings::shouldImportCamera(const QString& name) const
{
  return hasMatch(name, camerasToImport) || hasMatch(name+"-camera", camerasToImport);
}

bool SceneGraphImportSettings::shouldImportNode(const QString& name) const
{
  return hasMatch(name, nodesToImport) || hasMatch(name+"-node", nodesToImport);
}

float SceneGraphImportSettings::meshVoxelizeScaleFactorForMeshName(const QString& name) const
{
  for(auto i=meshVoxelizeScaleFactors.begin(); i!=meshVoxelizeScaleFactors.end(); ++i)
  {
    if(hasMatch(name, i.key()))
      return i.value();
  }

  return 1.f;
}

bool hasMatch(const QString& name, const QString& pattern)
{
  return QRegularExpression(pattern).match(name).hasMatch();
}

bool hasMatch(const QString& name, const QSet<QString>& patternsToImport)
{
  for(const QString& pattern : patternsToImport)
  {
    if(hasMatch(name, pattern))
      return true;
  }

  return false;
}

bool hasMatch(const aiString& name, const QSet<QString>& patternsToImport)
{
  return hasMatch(QString(name.C_Str()), patternsToImport);
}

SceneGraphImportSettings::SceneGraphImportSettings(AngelScriptInterface* interface)
{
  int uuidTypeId = angelScriptEngine->GetTypeIdByDecl("BaseUuid");
  int staticMeshUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<StaticMesh>");
  int materialUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Material>");
  int lightUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<LightSource>");
  int nodeUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<Node>");
  int cameraUuidTypeId = angelScriptEngine->GetTypeIdByDecl("Uuid<CameraComponent>");

  meshesToImport = AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToImport);
  camerasToImport = AngelScriptIntegration::scriptArrayToStringSet(interface->as_camerasToImport);
  nodesToImport = AngelScriptIntegration::scriptArrayToStringSet(interface->as_nodesToImport);
  meshesToVoxelize = AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToVoxelize);
  meshesNoToVoxelize = AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesNotToVoxelize);
  meshesToVoxelizeTwoSided = AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToVoxelizeTwoSided);
  meshesToVoxelizeWithManifold = AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToVoxelizeWithManifold);
  for(const QString& s : AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToMergeWhenVoxelizing).toList())
    meshesToMergeWhenVoxelizing.append(s.split("\n").toSet());
  for(const QString& s : AngelScriptIntegration::scriptArrayToStringSet(interface->as_meshesToMergeWhenVoxelizingInstanced).toList())
    meshesToMergeWhenVoxelizingInstanced.append(s.split("\n").toSet());

  meshVoxelizeProxies = AngelScriptIntegration::scriptDictionaryToHash<Uuid<StaticMesh>>(interface->as_meshVoxelizeProxies, {uuidTypeId, staticMeshUuidTypeId});
  meshVoxelizeScaleFactors = AngelScriptIntegration::scriptDictionaryToFloatHash<float>(interface->as_meshVoxelizeScaleFactors);
  meshUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<StaticMesh>>(interface->as_meshUuids, {uuidTypeId, staticMeshUuidTypeId});
  materialUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<Material>>(interface->as_materialUuids, {uuidTypeId, materialUuidTypeId});
  lightUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<LightSource>>(interface->as_lightUuids, {uuidTypeId, lightUuidTypeId});
  nodeUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<Node>>(interface->as_nodeUuids, {uuidTypeId, nodeUuidTypeId});
  cameraUuids = AngelScriptIntegration::scriptDictionaryToHash<Uuid<CameraParameter>>(interface->as_cameraUuids, {uuidTypeId, cameraUuidTypeId});

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
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesToVoxelize", asOFFSET(AngelScriptInterface,as_meshesToVoxelize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesNotToVoxelize", asOFFSET(AngelScriptInterface,as_meshesNotToVoxelize)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesToVoxelizeTwoSided", asOFFSET(AngelScriptInterface,as_meshesToVoxelizeTwoSided)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesToVoxelizeWithManifold", asOFFSET(AngelScriptInterface,as_meshesToVoxelizeWithManifold)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesToMergeWhenVoxelizing", asOFFSET(AngelScriptInterface,as_meshesToMergeWhenVoxelizing)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "array<string>@ meshesToMergeWhenVoxelizingInstanced", asOFFSET(AngelScriptInterface,as_meshesToMergeWhenVoxelizingInstanced)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ meshVoxelizeProxies", asOFFSET(AngelScriptInterface,as_meshVoxelizeProxies)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ meshVoxelizeScaleFactors", asOFFSET(AngelScriptInterface,as_meshVoxelizeScaleFactors)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ meshUuids", asOFFSET(AngelScriptInterface,as_meshUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ materialUuids", asOFFSET(AngelScriptInterface,as_materialUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ lightUuids", asOFFSET(AngelScriptInterface,as_lightUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ nodeUuids", asOFFSET(AngelScriptInterface,as_nodeUuids)); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "dictionary@ cameraUuids", asOFFSET(AngelScriptInterface,as_cameraUuids)); AngelScriptCheck(r);

  r = angelScriptEngine->RegisterGlobalProperty("bool forceReimport", &forceReimport_Assets); AngelScriptCheck(r);
}


void MeshImportSettings::registerType()
{
  int r;
  const char* name = "MeshImportSettings";

  r = angelScriptEngine->RegisterObjectType(name, sizeof(MeshImportSettings), AngelScript::asOBJ_VALUE|AngelScript::asOBJ_POD); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("MeshImportSettings", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(&AngelScriptIntegration::wrap_constructor<MeshImportSettings>), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectProperty(name, "bool indexed", asOFFSET(MeshImportSettings,indexed)); AngelScriptCheck(r);
}



} // namespace resources
} // namespace scene
} // namespace glrt

