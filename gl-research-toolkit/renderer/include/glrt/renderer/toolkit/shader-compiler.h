#ifndef GLRT_RENDERER_SHADERCOMPILER_H
#define GLRT_RENDERER_SHADERCOMPILER_H

#include <glrt/dependencies.h>

#include <glrt/renderer/gl/program.h>
#include <glrt/renderer/gl/shader-type.h>

namespace glrt {
namespace renderer {

class ShaderCompiler
{
public:
  struct CompileSettings
  {
    QString targetBinaryFile;
    QString name;
    QDir shaderDir;
    QStringList preprocessorBlock;

    bool operator==(const CompileSettings& other) const;

    QString toString() const;
    static CompileSettings fromString(const QString& encodedString);
  };

  QStringList preprocessorBlock;

  ShaderCompiler();

  static gl::Program compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static void compileProgramFromFiles_SaveBinary(const QString& targetBinaryFile, const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  static void compileProgramFromFiles_SaveBinary(const CompileSettings& settings);
  static void compileProgramFromFiles_SaveBinary_SubProcess(const CompileSettings& settings);

private:
  bool compile(gl::ShaderObject* shaderObject, const QDir& shaderDir);
  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static const QMap<QString, gl::ShaderType>& shaderTypes();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERCOMPILER_H
