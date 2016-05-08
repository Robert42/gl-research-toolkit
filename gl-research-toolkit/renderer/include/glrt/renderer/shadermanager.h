#ifndef GLRT_RENDERER_SHADERMANAGER_H
#define GLRT_RENDERER_SHADERMANAGER_H

#include "managed-shader.h"

#include <QFileSystemWatcher>
#include <QBitArray>

namespace glrt {
namespace renderer {

class ShaderManager : QObject
{
  Q_OBJECT
public:
  ShaderManager();
  ~ShaderManager();

  void addShaderSourceDir(const QDir& shaderDir);

private:
  enum class FileId : quint32
  {
    NONE=0,
  };
  enum class ProgramId : quint32
  {
    NONE=0,
  };

  struct ShaderFileIndex
  {
    QHash<QString, FileId> fileIds;
    QHash<FileId, QString> files;
    QHash<FileId, ProgramId> programForFile;
    QHash<ProgramId, QSet<FileId>> filesForProgram;
    QHash<FileId, QSet<FileId>> fileIncludes;
    QHash<FileId, QSet<FileId>> fileIncludedBy;

    FileId registerShaderFile(const QFileInfo& fileInfo);
    FileId idForShaderFile(const QFileInfo& fileInfo) const;

    ProgramId addFileToProgram(ProgramId program, FileId fileId);
    void updateIncludeGraph(FileId fileId);
  };

  ShaderFileIndex shaderFileIndex;
  QFileSystemWatcher fileSystemWatcher;

  QList<QDir> shaderSourceDirs; // directories, where to look for shaders with the extensions shaderExtensions


  friend uint qHash(FileId f){return ::qHash(quint32(f));}
  friend uint qHash(ProgramId p){return ::qHash(quint32(p));}

};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERMANAGER_H
