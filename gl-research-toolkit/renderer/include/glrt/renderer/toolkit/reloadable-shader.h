#ifndef GLRT_RENDERER_RELOADABLESHADER_H
#define GLRT_RENDERER_RELOADABLESHADER_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {
namespace renderer {

class ReloadableShader final
{
public:
  static QSet<QString> globalPreprocessorBlock;
  QSet<QString> preprocessorBlock;

  gl::ShaderObject shaderObject;

  ReloadableShader(const QString& name, const QDir& shaderDir, const QSet<QString>& preprocessorBlock=QSet<QString>());
  ReloadableShader(ReloadableShader&& other);
  ~ReloadableShader();

  ReloadableShader(const ReloadableShader&) = delete;
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

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_RELOADABLESHADER_H
