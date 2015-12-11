#include <glrt/toolkit/reloadable-shader.h>

namespace glrt {


ReloadableShader::ReloadableShader()
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
  return true;
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


} // namespace glrt

