#ifndef GLRT_RELOADABLESHADER_H
#define GLRT_RELOADABLESHADER_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {

class ReloadableShader final
{
public:
  static QSet<QString> globalPreprocessorBlock;
  QSet<QString> preprocessorBlock;

  gl::ShaderObject shaderObject;

  ReloadableShader(const QString& name, const QDir& shaderDir, const QSet<QString>& preprocessorBlock=QSet<QString>());
  ~ReloadableShader();

  ReloadableShader(const ReloadableShader&) = delete;
  ReloadableShader(ReloadableShader&&) = delete;
  ReloadableShader& operator=(const ReloadableShader&) = delete;
  ReloadableShader& operator=(ReloadableShader&&) = delete;

  bool reload();

  static void reloadAll();

private:
  const QDir shaderDir;

  static QSet<ReloadableShader*>& allReloadableShader();

  static bool reload(QSet<ReloadableShader*> shaders);

  QStringList wholeProprocessorBlock() const;
};

} // namespace glrt

#endif // GLRT_RELOADABLESHADER_H
