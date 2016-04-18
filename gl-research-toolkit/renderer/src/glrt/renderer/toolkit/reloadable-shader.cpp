#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

namespace glrt {
namespace renderer {


QSet<QString> ReloadableShader::globalPreprocessorBlock;

inline QString preprocessorForMacroName(const QString& macro)
{
  return QString("#define %0").arg(macro);
}

bool ReloadableShader::isMacroDefined(const QString& macro)
{
  return globalPreprocessorBlock.contains(preprocessorForMacroName(macro));
}

void ReloadableShader::defineMacro(const QString& macro, bool defined)
{
  QString proprocessor = preprocessorForMacroName(macro);

  if(defined)
    globalPreprocessorBlock.insert(proprocessor);
  else
    globalPreprocessorBlock.remove(proprocessor);
}


ReloadableShader::ReloadableShader(const QString& name, const QDir& shaderDir, const QSet<QString>& preprocessorBlock)
  : preprocessorBlock(preprocessorBlock),
    shaderObject(std::move(ShaderCompiler::createShaderFromFiles(name, shaderDir, wholeProprocessorBlock()))),
    shaderDir(shaderDir)
{
  allReloadableShader().insert(this);
}


ReloadableShader::ReloadableShader(ReloadableShader&& other)
  : preprocessorBlock(std::move(other.preprocessorBlock)),
    shaderObject(std::move(other.shaderObject)),
    shaderDir(other.shaderDir)
{
  allReloadableShader().insert(this);
  other.shaderDir = QDir();
}

ReloadableShader& ReloadableShader::operator=(ReloadableShader&& other)
{
  preprocessorBlock = std::move(other.preprocessorBlock);
  shaderObject = std::move(other.shaderObject);
  shaderDir = std::move(other.shaderDir);
  return *this;
}

ReloadableShader::~ReloadableShader()
{
  allReloadableShader().remove(this);
}


QSet<ReloadableShader*>& ReloadableShader::allReloadableShader()
{
  static QSet<ReloadableShader*> reloadableShader;

  return reloadableShader;
}


QSet<ReloadableShader::Listener*>& ReloadableShader::allListeners()
{
  static QSet<Listener*> listeners;

  return listeners;
}


bool ReloadableShader::reload()
{
  ShaderCompiler compiler;
  compiler.preprocessorBlock = wholeProprocessorBlock();

  return compiler.recompile(&this->shaderObject, shaderDir);
}


void ReloadableShader::reloadAll()
{
  bool succeeded = false;

  do
  {
    succeeded = reload(allReloadableShader());
  }while(!succeeded);

  for(Listener* listener : allListeners())
  {
    listener->allShadersReloaded();
  }
}


bool ReloadableShader::reload(QSet<ReloadableShader*> shaders)
{
  for(ReloadableShader* shader : shaders)
    if(!shader->reload())
      return false;

  return true;
}

QStringList ReloadableShader::wholeProprocessorBlock() const
{
  return (globalPreprocessorBlock|preprocessorBlock).toList();
}


// ========


ReloadableShader::Listener::Listener()
{
  allListeners().insert(this);
}

ReloadableShader::Listener::~Listener()
{
  allListeners().remove(this);
}


} // namespace renderer
} // namespace glrt
