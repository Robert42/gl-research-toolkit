#include <glrt/toolkit/reloadable-shader.h>
#include <glrt/toolkit/shader-compiler.h>

namespace glrt {


QSet<QString> ReloadableShader::globalPreprocessorBlock;


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


} // namespace glrt

