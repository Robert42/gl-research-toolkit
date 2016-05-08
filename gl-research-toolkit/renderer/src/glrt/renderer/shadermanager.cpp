#include <glrt/renderer/shadermanager.h>
#include <glrt/toolkit/recursive-dir-entry-info-list.h>
#include <QRegularExpression>

#include <glhelper/gl.hpp>

#include <QLinkedList>
#include <QCoreApplication>

namespace glrt {
namespace renderer {

const QStringList stringFileExtensions = {"*.cs", "*.fs", "*.vs"};

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::addShaderSourceDirs(const QList<QDir>& shaderDirs)
{
  // This function should be called only once
  Q_ASSERT(shaderSourceDirs.isEmpty());
  Q_ASSERT(shaderFileIndex.fileIds.isEmpty());
  Q_ASSERT(shaderFileIndex.files.isEmpty());

  this->shaderSourceDirs = shaderDirs;

  SPLASHSCREEN_MESSAGE(QString("Scanning directories for shaders"));

  QFileInfoList dirs;
  QFileInfoList files;

  for(const QDir& shaderDir : shaderDirs)
    recursiveDirEntryInfoList(shaderDir, dirs, files, QDir::Files, stringFileExtensions);

  for(const QFileInfo& f : files)
    shaderFileIndex.registerShaderFile(f);

  for(const QString& filePath : shaderFileIndex.fileIds.keys())
    fileSystemWatcher.addPath(filePath);
}

void ShaderManager::recompileProgramsNow()
{
  if(Q_UNLIKELY(!programsToRecompile.isEmpty()))
  {
    while(!programsToRecompile.isEmpty())
    {
      ProgramId program = programsToRecompile.first();

      if(recompileProgramNow(program))
        programsToRecompile.removeFirst();
      qApp->processEvents();
    }
  }
}

bool ShaderManager::recompileProgramNow(ProgramId program)
{
  qInfo() << "wouldRecompile";
  return true;
}

void ShaderManager::recompileProgramLater(ProgramId program)
{
  programsToRecompile.append(program);
}

void ShaderManager::handleChangedFile(const QString& filepath)
{
  FileId fileId = shaderFileIndex.idForShaderFile(filepath);
  shaderFileIndex.updateIncludeGraph(fileId);

  QSet<FileId> alreadyChecked;

  QLinkedList<FileId> filesToCheck;
  filesToCheck.append(fileId);
  while(!filesToCheck.isEmpty())
  {
    FileId fileToCheck = filesToCheck.first();
    filesToCheck.removeFirst();

    ProgramId program = shaderFileIndex.programForFile.value(fileToCheck, ProgramId::NONE);
    recompileProgramLater(program);

    alreadyChecked.insert(fileToCheck);
    for(FileId f : shaderFileIndex.fileIncludedBy.value(fileToCheck) - alreadyChecked)
      filesToCheck.append(f);
  }
}

ShaderManager::FileId ShaderManager::ShaderFileIndex::registerShaderFile(const QFileInfo& fileInfo)
{
  FileId fileId = idForShaderFile(fileInfo);

  if(fileId != FileId::NONE)
    return fileId;

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

  updateIncludeGraph(fileId);

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

void ShaderManager::ShaderFileIndex::updateIncludeGraph(FileId fileId)
{
  QFileInfo fileInfo = files.value(fileId);

  QSet<FileId>& fileIncludes = this->fileIncludes[fileId];

  for(FileId includededFile : fileIncludes)
  {
    Q_ASSERT(fileIncludedBy.contains(includededFile));
    Q_ASSERT(fileIncludedBy.value(includededFile).contains(fileId));

    this->fileIncludedBy[includededFile].remove(fileId);
  }

  fileIncludes.clear();
  QFile file(fileInfo.absoluteFilePath());

  if(file.exists() && file.open(QFile::ReadOnly))
  {
    static QRegularExpression includeRegex(R"([\n^]\s*\#include\s+([<"])([^">]+)([">]))");

    QString fileContent = QString::fromUtf8(file.readAll());

    QRegularExpressionMatchIterator matchIterator = includeRegex.globalMatch(fileContent);

    while(matchIterator.hasNext())
    {
      QRegularExpressionMatch match = matchIterator.next();

      QString fileLimiter = match.captured(1) + match.captured(3);
      QString fileToInclude = match.captured(2);
      QFileInfo includedFileInfo;

      if(fileLimiter == "\"\"")
        includedFileInfo = fileInfo.absoluteDir().absoluteFilePath(fileToInclude);
      else if(fileLimiter == "<>")
        includedFileInfo = gl::Details::ShaderIncludeDirManager::expandGlobalInclude(fileToInclude);
      else
        qWarning() << " ShaderManager::ShaderFileIndex::updateIncludeGraph: Invalid file limiter" << fileLimiter << "while including" << fileToInclude << "from" << fileInfo.absoluteFilePath();

      if(!includedFileInfo.isFile())
        qWarning() << " ShaderManager::ShaderFileIndex::updateIncludeGraph: Included not existign file" << includedFileInfo.absoluteFilePath() << "by including" << fileLimiter << "while including" << fileToInclude << "from" << fileInfo.absoluteFilePath();

      FileId includedFileId = registerShaderFile(includedFileInfo);
      fileIncludedBy[includedFileId].insert(fileId);
      fileIncludes.insert(includedFileId);
    }
  }
}


} // namespace renderer
} // namespace glrt
