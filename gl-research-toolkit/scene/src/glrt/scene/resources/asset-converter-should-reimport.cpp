#include <glrt/dependencies.h>

#include <QTemporaryDir>
#include <QProcess>
#include <QRegularExpression>
#include <QDateTime>


namespace glrt {
namespace scene {
namespace resources {

extern bool forceReimport_Assets;

bool forceReimport_Assets = false;

bool shouldConvert(const QFileInfo& targetFile, const QFileInfo& sourceFile, const QSet<QString>& converterSourceFile)
{
  if(forceReimport_Assets)
    return true;

  QDir targetDir = targetFile.path();
  QDir sourceDir = sourceFile.path();
  if(targetDir.exists("FORCE-REIMPORT"))
    return true;
  if(targetDir.exists(targetFile.baseName()+".FORCE-REIMPORT"))
    return true;
  if(sourceDir.exists(sourceFile.baseName()+".FORCE-REIMPORT"))
    return true;
  if(targetDir.exists(targetFile.fileName()+".FORCE-REIMPORT"))
    return true;
  if(sourceDir.exists(sourceFile.fileName()+".FORCE-REIMPORT"))
    return true;

  // No conversion possible if the source file doesn't exist
  if(!sourceFile.exists())
  {
    // If the target file also doesn't exist, print a warning
    if(!targetFile.exists())
      qWarning() << "Couldn't locate the asset file " << targetFile.filePath() << " (and neither the source file "<<sourceFile.filePath()<<" to automatically convert it)";

    return false;
  }

#if 0
#ifdef QT_DEBUG
  // If the cpp files responsible for converting the source files is newer than the target file, we probably need a reconversion, as the conversion code changed
  for(QFileInfo cppFile : converterSourceFile)
    if(cppFile.exists() && cppFile.lastModified() > targetFile.lastModified())
      return true;
#endif
#else
  Q_UNUSED(converterSourceFile);
#endif

  // automatically convert, if the target doesn't exist or the source file is newer
  return !targetFile.exists() || targetFile.lastModified() < sourceFile.lastModified();
}

} // namespace resources
} // namespace scene
} // namespace glrt
