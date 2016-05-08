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
  enum class MacroId : quint32
  {
    NONE=0,
  };

  ShaderManager();
  ~ShaderManager();

  void addShaderSourceDirs(const QList<QDir>& shaderDirs);
  void recompileProgramsNow();

  void setMacroEnabled(MacroId macro, bool enabled);
  bool isMacroEnabled(MacroId macro) const;

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

    QHash<FileId, QSet<MacroId>> macrosInFiles;
    QHash<MacroId, QSet<FileId>> filesUsingMacro;
    QHash<QString, MacroId> macroIds;
    QHash<MacroId, QString> macros;
    QSet<MacroId> existanceBasedMacros;
    QSet<MacroId> valueBasedMacros;

    MacroId registerExistenceBasedMacro(FileId fileId, const QString& macroName);
    MacroId registerValueBasedMacro(FileId fileId, const QString& macroName);

    FileId registerShaderFile(const QFileInfo& fileInfo);
    FileId idForShaderFile(const QFileInfo& fileInfo) const;

    ProgramId addFileToProgram(ProgramId program, FileId fileId);
    void updateProprocessorData(FileId fileId);
    void updateUsedMacros(FileId fileId);
    void updateIncludeGraph(FileId fileId);

  private:
    MacroId registerMacro(FileId fileId, const QString& name);
  };

  QSet<MacroId> enabledMacros;

  ShaderFileIndex shaderFileIndex;
  QFileSystemWatcher fileSystemWatcher;

  QList<QDir> shaderSourceDirs; // directories, where to look for shaders with the extensions shaderExtensions
  QList<ProgramId> programsToRecompile;

  friend uint qHash(FileId f){return ::qHash(quint32(f));}
  friend uint qHash(ProgramId p){return ::qHash(quint32(p));}
  friend uint qHash(MacroId m){return ::qHash(quint32(m));}

  bool recompileProgramNow(ProgramId program);
  void recompileProgramLater(ProgramId program);

  void recompileProgramsUsingFile(FileId fileId);

private slots:
  void handleChangedFile(const QString& filepath);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERMANAGER_H
