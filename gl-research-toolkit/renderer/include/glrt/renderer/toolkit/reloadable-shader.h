#ifndef GLRT_RENDERER_RELOADABLESHADER_H
#define GLRT_RENDERER_RELOADABLESHADER_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {
namespace renderer {

class ReloadableShader final
{
public:
  class Listener;

  static QSet<QString> globalPreprocessorBlock;
  QSet<QString> preprocessorBlock;

  gl::ShaderObject shaderObject;

  ReloadableShader(const QString& name, const QDir& shaderDir, const QSet<QString>& preprocessorBlock=QSet<QString>());
  ReloadableShader(ReloadableShader&& other);
  ~ReloadableShader();

  ReloadableShader(const ReloadableShader&) = delete;
  ReloadableShader& operator=(const ReloadableShader&) = delete;
  ReloadableShader& operator=(ReloadableShader&&);

  bool reload();

  static void reloadAll();

private:
  QDir shaderDir;

  static QSet<Listener*>& allListeners();
  static QSet<ReloadableShader*>& allReloadableShader();

  static bool reload(QSet<ReloadableShader*> shaders);

  QStringList wholeProprocessorBlock() const;
};


class ReloadableShader::Listener
{
public:
  Listener();
  virtual ~Listener();

  Listener(const Listener&) = delete;
  Listener& operator=(const Listener&) = delete;

  virtual void allShadersReloaded() = 0;
};

} // namespace renderer


template<>
struct DefaultTraits<renderer::ReloadableShader>
{
  typedef ArrayTraits_mCmOD<renderer::ReloadableShader> type;
};

} // namespace glrt

#endif // GLRT_RENDERER_RELOADABLESHADER_H
