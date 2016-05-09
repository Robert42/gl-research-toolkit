#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/toolkit/logger.h>

#include <glhelper/shaderobject.hpp>

#include <set>

#include <QRegularExpression>
#include <QTemporaryDir>
#include <QProcess>

namespace glrt {
namespace renderer {


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


ShaderCompiler::ShaderCompiler()
{
}


bool ShaderCompiler::compile(gl::ShaderObject* shaderObject, const QDir& shaderDir)
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
  QProcess::execute(QString(GLRT_SHADER_COMPILER_PATH), QStringList({settings.toString()}));
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

  ShaderCompiler compiler;

  compiler.preprocessorBlock = preprocessorBlock;

  while(!compiler.compile(&shaderObject, shaderDir));

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


} // namespace renderer
} // namespace glrt
