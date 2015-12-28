#include <glrt/scene/resources/asset-converter.h>
#include <QTemporaryDir>

namespace glrt {
namespace scene {
namespace resources {

void convertStaticMesh(const std::string& meshFilename, const std::string& sourceFilename)
{
  QFileInfo meshFile(QString::fromStdString(meshFilename));
  QFileInfo sourceFile(QString::fromStdString(sourceFilename));
  QTemporaryDir temporaryDir;

  if(!temporaryDir.isValid())
    throw GLRT_EXCEPTION("Couldn't convert file to static mesh");

  qDebug() << "convertStaticMesh("<<meshFile<<","<<sourceFile<<")";
}

} // namespace resources
} // namespace scene
} // namespace glrt

