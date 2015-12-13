#include <glrt/toolkit/shader-compiler.h>
#include <glrt/toolkit/temp-shader-file.h>
#include <glrt/toolkit/logger.h>

#include <set>

namespace glrt {


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

  bool handleMessage(QtMsgType type, const QMessageLogContext&, const QString& message)
  {
    if(type == QtDebugMsg)
      return true;

    return false;
  }

  ShaderErrorDialog(const ShaderErrorDialog&) = delete;
  ShaderErrorDialog(ShaderErrorDialog&&) = delete;
  ShaderErrorDialog& operator=(const ShaderErrorDialog&) = delete;
  ShaderErrorDialog& operator=(ShaderErrorDialog&&) = delete;

private:
 int stack_size;
};


ShaderCompiler::ShaderCompiler()
{
}


bool ShaderCompiler::compile(gl::ShaderObject* shaderObject, const QDir& shaderDir)
{
  ShaderErrorDialog errorDialog;

  TempShaderFile tempShaderFile;

  const QMap<QString, gl::ShaderObject::ShaderType>& shaderTypes = ShaderCompiler::shaderTypes();

  tempShaderFile.addPreprocessorBlock(preprocessorBlock);

  std::set<gl::ShaderObject::ShaderType> usedTypes;

  QString shaderName = QString::fromStdString(shaderObject->GetName());

  for(const QString& extension : shaderTypes.keys())
  {
    QFileInfo file = shaderDir.filePath(shaderName + extension);

    if(!file.exists())
      continue;

    gl::ShaderObject::ShaderType type = shaderTypes[extension];

    shaderObject->AddShaderFromFile(type, file.absoluteFilePath().toStdString());

    usedTypes.insert(type);
  }


  if(usedTypes.empty())
    throw GLRT_EXCEPTION(QString("No shader files found for %0").arg(shaderName));


  shaderObject->CreateProgram();

  return true;
}


bool ShaderCompiler::recompile(gl::ShaderObject* shaderObject, const QDir& shaderDir)
{
  return compile(shaderObject, shaderDir);
}


gl::ShaderObject ShaderCompiler::createShaderFromFiles(const QString &name, const QDir &shaderDir, const QStringList &preprocessorBlock)
{
  gl::ShaderObject shaderObject(name.toStdString());

  ShaderCompiler compiler;

  compiler.preprocessorBlock = preprocessorBlock;

  compiler.compile(&shaderObject, shaderDir);

  return std::move(shaderObject);
}


const QMap<QString, gl::ShaderObject::ShaderType>& ShaderCompiler::shaderTypes()
{
  static QMap<QString, gl::ShaderObject::ShaderType> shaderTypes;

  if(shaderTypes.isEmpty())
  {
    shaderTypes[".vs"] = gl::ShaderObject::ShaderType::VERTEX;
    shaderTypes[".fs"] = gl::ShaderObject::ShaderType::FRAGMENT;
  }

  return shaderTypes;
}


} // namespace glrt

