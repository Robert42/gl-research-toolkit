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
  if(!programsToRecompile.contains(program))
    programsToRecompile.append(program);
}

void ShaderManager::handleChangedFile(const QString& filepath)
{
  FileId fileId = shaderFileIndex.idForShaderFile(filepath);
  shaderFileIndex.updateProprocessorData(fileId);

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

inline bool suppressedMacro(const QString& macroName)
{
  static QSet<QString> not_macros({"in", "out", "inout", "defined", "__GNUC__"});

  return not_macros.contains(macroName);
}

ShaderManager::MacroId ShaderManager::ShaderFileIndex::registerExistenceBasedMacro(FileId fileId, const QString& macroName)
{
  if(suppressedMacro(macroName))
    return MacroId::NONE;

  // #TODO::::::::
  qInfo() << "registerExistenceBasedMacro" << macroName;
  return MacroId::NONE;
}

ShaderManager::MacroId ShaderManager::ShaderFileIndex::registerValueBasedMacro(FileId fileId, const QString& macroName)
{
  if(suppressedMacro(macroName))
    return MacroId::NONE;

  // #TODO::::::::
  qInfo() << "registerValueBasedMacro" << macroName;
  return MacroId::NONE;
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

  updateProprocessorData(fileId);

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

void ShaderManager::ShaderFileIndex::updateProprocessorData(FileId fileId)
{
  updateIncludeGraph(fileId);
  updateUsedMacros(fileId);
}

void ShaderManager::ShaderFileIndex::updateUsedMacros(FileId fileId)
{
  QFileInfo fileInfo = files.value(fileId);
  QFile file(fileInfo.absoluteFilePath());

  if(file.exists() && file.open(QFile::ReadOnly))
  {
    static QRegularExpression ifRegex(R"([\n^]\s*\#(if\S*)\s+([^\n$]+)\s*[\n$])");
    static QRegularExpression macro_name("[_a-zA-Z][_a-zA-Z0-9]*");

    QString fileContent = QString::fromUtf8(file.readAll());

    QRegularExpressionMatchIterator matchIterator = ifRegex.globalMatch(fileContent);

    while(matchIterator.hasNext())
    {
      QRegularExpressionMatch match = matchIterator.next();

      QString ifType = match.captured(1);
      QString ifContent = match.captured(2).trimmed();

      if(ifType == "ifdef" || ifType == "ifndef")
        registerExistenceBasedMacro(fileId, ifContent);
      else if(ifType == "if")
      {
        QRegularExpressionMatchIterator matchIterator = macro_name.globalMatch(ifContent);
        while(matchIterator.hasNext())
        {
          QRegularExpressionMatch match = matchIterator.next();
          registerValueBasedMacro(fileId, match.captured(0));
        }
      }else
        qWarning() << "Invalid #if expression " << match.captured(0);
    }
  }
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
