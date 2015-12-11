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


} // namespace glrt

