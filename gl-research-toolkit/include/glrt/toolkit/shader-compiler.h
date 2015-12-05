#ifndef GLRT_SHADERCOMPILER_H
#define GLRT_SHADERCOMPILER_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {

class ShaderCompiler
{
public:
  QStringList preprocessorBlock;

  ShaderCompiler();

  void compile(gl::ShaderObject* shaderObject, const QDir& shaderDir);

  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

private:
  static const QMap<QString, gl::ShaderObject::ShaderType>& shaderTypes();
};

} // namespace glrt

#endif // GLRT_SHADERCOMPILER_H
