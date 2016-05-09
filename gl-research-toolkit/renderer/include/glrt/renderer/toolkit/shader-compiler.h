#ifndef GLRT_RENDERER_SHADERCOMPILER_H
#define GLRT_RENDERER_SHADERCOMPILER_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>
#include <glrt/renderer/gl/program.h>
#include <glrt/renderer/gl/shader-type.h>

namespace glrt {
namespace renderer {

class ShaderCompiler
{
public:
  QStringList preprocessorBlock;

  ShaderCompiler();

  bool compile(gl::ShaderObject* shaderObject, const QDir& shaderDir);
  bool recompile(gl::ShaderObject* shaderObject, const QDir& shaderDir);

  static gl::Program compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static void compileProgramFromFiles_SaveBinary(const QString& targetBinaryFile, const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

private:
  static const QMap<QString, gl::ShaderType>& shaderTypes();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERCOMPILER_H
