#ifndef GLRT_RELOADABLESHADER_H
#define GLRT_RELOADABLESHADER_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {

class ReloadableShader final
{
public:
  ReloadableShader();
  ~ReloadableShader();

  ReloadableShader(const ReloadableShader&) = delete;
  ReloadableShader(ReloadableShader&&) = delete;
  ReloadableShader& operator=(const ReloadableShader&) = delete;
  ReloadableShader& operator=(ReloadableShader&&) = delete;

  bool reload();

  static void reloadAll();

private:
  static QSet<ReloadableShader*>& allReloadableShader();

  static bool reload(QSet<ReloadableShader*> shaders);
};

} // namespace glrt

#endif // GLRT_RELOADABLESHADER_H
