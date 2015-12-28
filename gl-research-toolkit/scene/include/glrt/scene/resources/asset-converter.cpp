#include <glrt/scene/resources/asset-converter.h>
#include <QTemporaryDir>
#include <QProcess>

namespace glrt {
namespace scene {
namespace resources {

void convertStaticMesh_assimpToMesh(const QFileInfo& meshFile, const QFileInfo& sourceFile);

void runBlenderWithPythonScript(const QString& pythonScript, const QFileInfo& blenderFile);
QString python_exportSceneAsObj(const QString& objFile);

void convertStaticMesh_BlenderToCollada(const QFileInfo& meshFile, const QFileInfo& blenderFile)
{
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to static mesh");

  QString tempFilePath = QDir(temporaryDir.path()).absoluteFilePath("exported.obj");

  runBlenderWithPythonScript(python_exportSceneAsObj(tempFilePath), blenderFile);

  convertStaticMesh_assimpToMesh(meshFile, tempFilePath);
}

void convertStaticMesh(const std::string& meshFilename, const std::string& sourceFilename)
{
  // #TODO update splashscreen (use a raii for splashscreen messages?)

  QFileInfo meshFile(QString::fromStdString(meshFilename));
  QFileInfo sourceFile(QString::fromStdString(sourceFilename));

  // #TODO comment in again
  //if(meshFile.lastModified() < sourceFile.lastModified())
  {
    if(sourceFile.suffix().toLower() == "blend")
      convertStaticMesh_BlenderToCollada(meshFile, sourceFile);
    else
      convertStaticMesh_assimpToMesh(meshFile, sourceFile);
  }
}


void runBlenderWithPythonScript(const QString& pythonScript, const QFileInfo& blenderFile)
{
  // #TODO update splashscreen (use a raii for splashscreen messages?)

  QString blenderProgram("blender"); // #TODO, search for blender instead of just assuming it in the PATH variable?
  QStringList arguments = {"--background", blenderFile.absoluteFilePath(), "--python-expr", pythonScript};

  if(QProcess::execute(blenderProgram, arguments) !=0)
    throw GLRT_EXCEPTION("Executing Blender failed");
}

QString to_python_string(QString str)
{
  return str.replace("\\", R"("'\\'")").replace("\"", "\\\"").prepend("r\"").append("\"");
}

QString python_exportSceneAsObj(const QString& objFile)
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

void convertStaticMesh_assimpToMesh(const QFileInfo& meshFile, const QFileInfo& sourceFile)
{
  // #TODO update splashscreen (use a raii for splashscreen messages?)
  qDebug() << "convertStaticMesh_assimpToMesh("<<meshFile.absoluteFilePath()<<","<<sourceFile.absoluteFilePath()<<")";
}

} // namespace resources
} // namespace scene
} // namespace glrt

