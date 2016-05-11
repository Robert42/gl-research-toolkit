#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/toolkit/logger.h>

#include <glhelper/shaderobject.hpp>

#include <set>

#include <QRegularExpression>
#include <QTemporaryDir>
#include <QThread>
#include <QSharedMemory>
#include <QTcpSocket>

namespace glrt {
namespace renderer {

ShaderCompiler* ShaderCompiler::_singleton = nullptr;

class ShaderErrorDialog final
{
public:
  ShaderErrorDialog()
  {
    Logger::handler.push(std::bind(&ShaderErrorDialog::handleMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    stack_size = Logger::handler.size();
  }

  ~ShaderErrorDialog()
  {
    Q_ASSERT(stack_size==Logger::handler.size());
    Logger::handler.pop();
  }

  bool thereWereErrors()
  {
    if(messages.isEmpty())
      return false;

    QString message = messages.join("\n\n");

    int len = 2048;
    if(message.length() > len)
    {
      QString noteAboutRemovedLines("\n\n[...] Rest of the message (%0 lines) removed");

      int linesVisibleBefore = message.count(QChar('\n'))+1;
      message.resize(len-(noteAboutRemovedLines.length()+5));
      int linesVisibleNow = message.count(QChar('\n'))+1;
      int linesRemoved = linesVisibleBefore-linesVisibleNow;

      message += noteAboutRemovedLines.arg(linesRemoved);
    }

    enum Results : int
    {
      INGORE,
      EXIT,
      RECOMPILE
    };

    const SDL_MessageBoxButtonData buttons[] = {
      {0, 0, "Ignore"},
      {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Exit App"},
      {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 2, "Recompile Shaders"},
    };

    std::string std = message.toStdString();

    SDL_MessageBoxData msgBoxData = {SDL_MESSAGEBOX_ERROR, nullptr, "Shader Compile Error", std.c_str(), SDL_arraysize(buttons), buttons, nullptr};

    int result;
    if(SDL_ShowMessageBox(&msgBoxData, &result) < 0)
    {
      std::cerr << "Displayng SHader Compile Error Dialog failed"<<std::endl;
      std::exit(-1);
    }

    if(result == EXIT)
    {
      std::cout << "Aborted by user"<<std::endl;
      std::exit(-1);
    }

    return result!=INGORE;
  }

  ShaderErrorDialog(const ShaderErrorDialog&) = delete;
  ShaderErrorDialog(ShaderErrorDialog&&) = delete;
  ShaderErrorDialog& operator=(const ShaderErrorDialog&) = delete;
  ShaderErrorDialog& operator=(ShaderErrorDialog&&) = delete;

private:
 int stack_size;

 QStringList messages;

  bool handleMessage(QtMsgType type, const QMessageLogContext&, const QString& message)
  {
    if(type == QtDebugMsg)
      return true;

    QString improvedMessage = message;

    improvedMessage = improvedMessage.replace("compiled.Output:", "compiled with errors:");

    messages << improvedMessage;

    return false;
 }
};


ShaderCompiler::ShaderCompiler(bool startServer)
{
  Q_ASSERT(_singleton == nullptr);
  _singleton = this;

  if(startServer)
    startCompileProcess();
}

ShaderCompiler::~ShaderCompiler()
{
  endCompileProcess();

  Q_ASSERT(_singleton == this);
  _singleton = nullptr;
}


ShaderCompiler& ShaderCompiler::singleton()
{
  Q_ASSERT(_singleton != nullptr);
  return *_singleton;
}


bool ShaderCompiler::compile(gl::ShaderObject* shaderObject, const QDir& shaderDir, const QStringList& preprocessorBlock)
{
  SPLASHSCREEN_MESSAGE("compile Shader");

  ShaderErrorDialog errorDialog;

  const QMap<QString, gl::ShaderType>& shaderTypes = ShaderCompiler::shaderTypes();

  std::string prefixCode = preprocessorBlock.join("\n").toStdString();

  std::set<gl::ShaderType> usedTypes;

  QString shaderName = QString::fromStdString(shaderObject->GetName());

  for(const QString& extension : shaderTypes.keys())
  {
    QFileInfo file = shaderDir.filePath(shaderName + extension);

    if(!file.exists())
      continue;

    gl::ShaderType type = shaderTypes[extension];

    shaderObject->AddShaderFromFile(type, file.absoluteFilePath().toStdString(), prefixCode);

    usedTypes.insert(type);
  }


  if(usedTypes.empty())
    throw GLRT_EXCEPTION(QString("No shader files found for %0").arg(shaderName));


  shaderObject->CreateProgram();

  return !errorDialog.thereWereErrors();
}


gl::Program ShaderCompiler::compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock)
{
  QTemporaryDir tempDir;

  if(!tempDir.isValid())
    throw GLRT_EXCEPTION(QString("Couldn't create").arg(name));

  QString binaryProgramFile = QDir(tempDir.path()).absoluteFilePath("binary-gl-program-file");

  CompileSettings settings;
  settings.targetBinaryFile = binaryProgramFile;
  settings.name = name;
  settings.shaderDir = shaderDir;
  settings.preprocessorBlock = preprocessorBlock;
  compileProgramFromFiles_SaveBinary_SubProcess(settings);

  gl::Program program;
  program.loadFromFile(binaryProgramFile);

  return program;
}


void ShaderCompiler::compileProgramFromFiles_SaveBinary_SubProcess(const CompileSettings& settings)
{
  SPLASHSCREEN_MESSAGE(QString("Compiling Shader %0").arg(settings.name));
  qInfo() << "Compiling Shader" << settings.name;

  Q_ASSERT(CompileSettings::fromString(settings.toString()) == settings);
  Q_ASSERT(CompileSettings::fromStringList(settings.toStringList()) == settings);

  QFileInfo newShaderFile(settings.targetBinaryFile);

  QByteArray command = settings.toStringList().join('\n').toUtf8();

  startCompileProcess();
  tcpConnection->write(command);
  tcpConnection->flush();

  while(!newShaderFile.exists())
  {
    if(!compilerProcessIsRunning())
    {
      startCompileProcess();
      tcpConnection->write(command);
      tcpConnection->flush();
    }

    QThread::msleep(1);
  }
}

bool ShaderCompiler::compilerProcessIsRunning() const
{
  return compileProcess.state() == QProcess::Running && tcpConnection!=nullptr && tcpConnection->isOpen();;
}

void ShaderCompiler::startCompileProcess()
{
  if(!compilerProcessIsRunning())
  {
    if(nProcessStarted != 0)
      qWarning() << "Compile process didn't respond, creating a new compile process";
    ++nProcessStarted;

    endCompileProcess();

    tcpServer.listen(QHostAddress::LocalHost, GLRT_SHADER_COMPILER_PORT);

    compileProcess.start(QString(GLRT_SHADER_COMPILER_PATH), QIODevice::NotOpen);
    compileProcess.setReadChannel(QProcess::StandardOutput);

    if(!compileProcess.waitForStarted())
      throw GLRT_EXCEPTION("Couldn't start compiler process");

    if(!tcpServer.waitForNewConnection(30000))
      throw GLRT_EXCEPTION("Couldn't connect to the compiler process (error code: 0x6254)");

    if(!tcpServer.hasPendingConnections())
      throw GLRT_EXCEPTION("Couldn't connect to the compiler process (error code: 0x3514)");

    delete this->tcpConnection;
    this->tcpConnection = tcpServer.nextPendingConnection();

    if(tcpServer.hasPendingConnections())
      throw GLRT_EXCEPTION("Couldn't connect to the compiler process (error code: 0x6817)");

    if(!compilerProcessIsRunning())
      throw GLRT_EXCEPTION("Couldn't connect to the compiler process (error code: 0x57475)");
  }
}

void ShaderCompiler::endCompileProcess()
{
  if(tcpConnection)
  {
    delete this->tcpConnection;
    tcpServer.close();
  }
  if(compileProcess.state() == QProcess::Running)
  {
    compileProcess.waitForFinished(30000);
    compileProcess.terminate();
    compileProcess.close();
  }
}

void ShaderCompiler::compileProgramFromFiles_SaveBinary(const QString& targetBinaryFile, const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock)
{
  gl::ShaderObject shaderObject = createShaderFromFiles(name, shaderDir, preprocessorBlock);

  gl::Program::saveShaderObjectToFile(targetBinaryFile, &shaderObject);
}


void ShaderCompiler::compileProgramFromFiles_SaveBinary(const CompileSettings& settings)
{
  compileProgramFromFiles_SaveBinary(settings.targetBinaryFile, settings.name, settings.shaderDir, settings.preprocessorBlock);
}


gl::ShaderObject ShaderCompiler::createShaderFromFiles(const QString &name, const QDir &shaderDir, const QStringList &preprocessorBlock)
{
  gl::ShaderObject shaderObject(name.toStdString());

  ShaderCompiler& compiler = ShaderCompiler::singleton();

  while(!compiler.compile(&shaderObject, shaderDir, preprocessorBlock));

  return std::move(shaderObject);
}


const QMap<QString, gl::ShaderType>& ShaderCompiler::shaderTypes()
{
  static QMap<QString, gl::ShaderType> shaderTypes;

  if(shaderTypes.isEmpty())
  {
    shaderTypes[".cs"] = gl::ShaderType::COMPUTE;
    shaderTypes[".vs"] = gl::ShaderType::VERTEX;
    shaderTypes[".fs"] = gl::ShaderType::FRAGMENT;
  }

  return shaderTypes;
}


bool ShaderCompiler::CompileSettings::operator==(const ShaderCompiler::CompileSettings& other) const
{
  return this->targetBinaryFile == other.targetBinaryFile
      && this->name == other.name
      && this->shaderDir == other.shaderDir
      && this->preprocessorBlock == other.preprocessorBlock;
}

QString ShaderCompiler::CompileSettings::toString() const
{
  QStringList values;
  values << this->name;
  values << this->shaderDir.absolutePath();
  values << this->targetBinaryFile;
  values << this->preprocessorBlock;

  return values.join("\n");
}

ShaderCompiler::CompileSettings ShaderCompiler::CompileSettings::fromString(const QString& encodedString)
{
  ShaderCompiler::CompileSettings settings;

  QStringList list = encodedString.split('\n');

  settings.name = list.first();
  list.removeFirst();

  settings.shaderDir = list.first();
  list.removeFirst();

  settings.targetBinaryFile = list.first();
  list.removeFirst();

  settings.preprocessorBlock = list;

  return settings;
}

QStringList ShaderCompiler::CompileSettings::toStringList() const
{
  QStringList values;
  values << this->name;
  values << this->shaderDir.absolutePath();
  values << this->targetBinaryFile;
  values << this->preprocessorBlock;
  values << QString();

  values.prepend(QString("ShaderCompiler::CompileSettings-%0").arg(values.length()));

  return values;
}

ShaderCompiler::CompileSettings ShaderCompiler::CompileSettings::fromStringList(QStringList encodedStringList)
{
  CompileSettings settings;
  bool success = fromStringList(settings, encodedStringList);
  Q_ASSERT(success);

  return settings;
}

bool ShaderCompiler::CompileSettings::fromStringList(ShaderCompiler::CompileSettings& settings, QStringList& encodedStringList)
{
  if(encodedStringList.isEmpty())
    return false;
  if(!encodedStringList.first().startsWith("ShaderCompiler::CompileSettings-"))
    return false;
  const int n = encodedStringList.first().split('-').last().toInt();

  if(n>encodedStringList.length())
    return false;

  encodedStringList.removeFirst();

  QStringList list;
  for(int i=1; i<n; ++i)
  {
    list << encodedStringList.first();
    encodedStringList.removeFirst();
  }

  settings.name = list.first();
  list.removeFirst();

  settings.shaderDir = list.first();
  list.removeFirst();

  settings.targetBinaryFile = list.first();
  list.removeFirst();

  settings.preprocessorBlock = list;
  return true;
}


} // namespace renderer
} // namespace glrt
