#include <glrt/renderer/shadermanager.h>
#include <glrt/toolkit/recursive-dir-entry-info-list.h>

namespace glrt {
namespace renderer {

const QStringList stringFileExtensions = {"*.cs", "*.fs", "*.vs"};

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::addShaderSourceDir(const QDir& shaderDir)
{
  QFileInfoList dirs;
  QFileInfoList files;

  recursiveDirEntryInfoList(shaderDir, dirs, files, QDir::Files, stringFileExtensions);

  for(const QFileInfo& f : files)
  {
    shaderFileIndex.registerShaderFile(f);
  }
}

ShaderManager::FileId ShaderManager::ShaderFileIndex::registerShaderFile(const QFileInfo& fileInfo)
{
  FileId fileId = idForShaderFile(fileInfo);

  if(fileId != FileId::NONE)
  {
    Q_UNREACHABLE();
    return fileId;
  }

  fileId = FileId(fileIds.size()+1);

  QString absolutePath = fileInfo.absoluteFilePath();

  fileIds.insert(absolutePath, fileId);
  files.insert(fileId, absolutePath);

  QDir path = fileInfo.absoluteDir();
  QString basename = fileInfo.baseName();
  QString suffix = fileInfo.suffix();

  bool isPartOfProgram = stringFileExtensions.contains("*."+suffix);

  if(isPartOfProgram)
  {
    ProgramId program = ProgramId::NONE;

    for(const QString& s : stringFileExtensions)
    {
      QString otherFile = path.absoluteFilePath(QString(s).replace('*', basename));

      program = programForFile.value(fileIds.value(otherFile, FileId::NONE), ProgramId::NONE);

      if(program != ProgramId::NONE)
        break;
    }

    addFileToProgram(program, fileId);
  }

  return fileId;
}

ShaderManager::FileId ShaderManager::ShaderFileIndex::idForShaderFile(const QFileInfo& fileInfo) const
{
  return fileIds.value(fileInfo.absoluteFilePath(), FileId::NONE);
}

ShaderManager::ProgramId ShaderManager::ShaderFileIndex::addFileToProgram(ProgramId program, FileId fileId)
{
  if(program == ProgramId::NONE)
    program = ProgramId(filesForProgram.size()+1);

  programForFile.insert(fileId, program);
  filesForProgram[program].insert(fileId);

  return program;
}


} // namespace renderer
} // namespace glrt
