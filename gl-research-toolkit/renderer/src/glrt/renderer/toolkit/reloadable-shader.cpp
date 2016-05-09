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

void ReloadableShader::defineMacro(const QString& macro, bool defined, bool autoReloadShaders)
{
  QString proprocessor = preprocessorForMacroName(macro);

  bool prevValue = isMacroDefined(macro);

  if(defined)
    globalPreprocessorBlock.insert(proprocessor);
  else
    globalPreprocessorBlock.remove(proprocessor);

  if(autoReloadShaders && prevValue != defined)
    reloadAll();
}


ReloadableShader::ReloadableShader(const QString& name, const QDir& shaderDir, const QSet<QString>& preprocessorBlock)
  : preprocessorBlock(preprocessorBlock),
    name(name),
    glProgram(std::move(ShaderCompiler::compileProgramFromFiles(name, shaderDir, wholeProprocessorBlock()))),
    shaderDir(shaderDir)
{
  allReloadableShader().insert(this);
}


ReloadableShader::ReloadableShader(ReloadableShader&& other)
  : preprocessorBlock(std::move(other.preprocessorBlock)),
    name(std::move(other.name)),
    glProgram(std::move(other.glProgram)),
    shaderDir(other.shaderDir)
{
  allReloadableShader().insert(this);
  other.shaderDir = QDir();
}

ReloadableShader& ReloadableShader::operator=(ReloadableShader&& other)
{
  preprocessorBlock = std::move(other.preprocessorBlock);
  glProgram = std::move(other.glProgram);
  name = std::move(other.name);
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

  gl::Program program = ShaderCompiler::compileProgramFromFiles(name, shaderDir, wholeProprocessorBlock());

  if(program.programId == 0)
    return false;

  std::swap(program, this->glProgram);

  return true;
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
