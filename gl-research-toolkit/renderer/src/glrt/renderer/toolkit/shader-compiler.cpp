#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/toolkit/logger.h>

#include <glhelper/shaderobject.hpp>

#include <set>

#include <QRegularExpression>
#include <QTemporaryDir>
#include <QThread>
#include <QSharedMemory>
#include <QTcpSocket>
#include <QElapsedTimer>

namespace glrt {
namespace renderer {

ShaderCompiler* ShaderCompiler::_singleton = nullptr;

std::function<void(ShaderCompiler::DialogAction)> ShaderCompiler::shaderDialogVisible = [](ShaderCompiler::DialogAction){};

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
    ShaderCompiler::shaderDialogVisible(ShaderCompiler::DialogAction::Show);
    bool dialogError = SDL_ShowMessageBox(&msgBoxData, &result) < 0;
    ShaderCompiler::shaderDialogVisible(ShaderCompiler::DialogAction::Hide);
    if(dialogError)
    {
      std::cerr << "Displayng SHader Compile Error Dialog failed"<<std::endl;
      std::exit(-1);
    }

    if(result == EXIT)
    {
      ShaderCompiler::shaderDialogVisible(ShaderCompiler::DialogAction::ExitApp);
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


void ShaderCompiler::registerGlrtShaderIncludeDirectories()
{
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_SHADER_DIR).absoluteFilePath("toolkit"));
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_SHADER_DIR).absoluteFilePath("common-with-cpp"));
  gl::Details::ShaderIncludeDirManager::addIncludeDirs(QDir(GLRT_EXTERNAL_SHADER_DIR).absolutePath());
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
  CompileSettings settings;
  settings.name = name;
  settings.shaderDir = shaderDir;
  settings.preprocessorBlock = preprocessorBlock;
  return compileProgramFromFiles_SubProcess(settings);
}


gl::Program ShaderCompiler::compileProgramFromFiles_SubProcess(const CompileSettings& settings)
{
  SPLASHSCREEN_MESSAGE(QString("Compiling Shader %0").arg(settings.name));
  qInfo() << "Compiling Shader" << settings.name;

  Q_ASSERT(CompileSettings::fromString(settings.toString()) == settings);

  glrt::TcpMessages messages;
  messages.connection = tcpConnection;

  TcpMessages::Message msgCompile;
  msgCompile.id = shaderCompileCommand;
  msgCompile.byteArray = settings.toString().toUtf8();

  startCompileProcess();
  messages.sendMessage(msgCompile);

  QElapsedTimer timer;
  timer.start();

  bool currentlyWaitingForUser = false;
  while(true)
  {
    if(!compilerProcessIsRunning())
    {
      startCompileProcess();
      messages.sendMessage(msgCompile);
    }

    if(!messages.waitForReadyRead(1000) && !currentlyWaitingForUser)
    {
      endCompileProcess();
      timer.restart();
    }

    if(messages.messageAvialable())
    {
      TcpMessages::Message msg = messages.readMessage();
      if(msg.id == glslBytecode)
      {
        gl::Program program;
        program.loadFromBinary(msg.byteArray);

        return program;
      }else if(msg.id == startedWaitingForUserInput)
      {
        currentlyWaitingForUser = true;
      }else if(msg.id == finishedWaitingForUserInput)
      {
        currentlyWaitingForUser = false;
      }else if(msg.id == exitApplication)
      {
        qInfo() << "Aborted by user";
        std::exit(0);
      }
    }
  }
}

bool ShaderCompiler::compilerProcessIsRunning() const
{
  return compileProcess.state() == QProcess::Running && tcpConnection!=nullptr && tcpConnection->isOpen();
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
    tcpConnection = nullptr;
  }
  if(compileProcess.state() == QProcess::Running)
  {
    compileProcess.waitForFinished(30000);
    compileProcess.terminate();
    compileProcess.close();
  }
}

QByteArray ShaderCompiler::compileProgramFromFiles_GetBinary(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock)
{
  gl::ShaderObject shaderObject = createShaderFromFiles(name, shaderDir, preprocessorBlock);

  return gl::Program::saveShaderObjectToByteArray(&shaderObject);
}


QByteArray ShaderCompiler::compileProgramFromFiles_GetBinary(const CompileSettings& settings)
{
  return compileProgramFromFiles_GetBinary(settings.name, settings.shaderDir, settings.preprocessorBlock);
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
  return this->name == other.name
      && this->shaderDir == other.shaderDir
      && this->preprocessorBlock == other.preprocessorBlock;
}

QString ShaderCompiler::CompileSettings::toString() const
{
  QStringList values;
  values << this->name;
  values << this->shaderDir.absolutePath();
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

  settings.preprocessorBlock = list;

  return settings;
}


} // namespace renderer
} // namespace glrt
