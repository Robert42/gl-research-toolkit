#ifndef GLRT_RENDERER_SHADERCOMPILER_H
#define GLRT_RENDERER_SHADERCOMPILER_H

#include <glrt/dependencies.h>

#include <glrt/renderer/gl/program.h>
#include <glrt/renderer/gl/shader-type.h>

#include <QProcess>

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

    QStringList toStringList() const;
    static bool fromStringList(CompileSettings& settings, QStringList& encodedStringList);
    QString toString() const;
    static CompileSettings fromString(const QString& encodedString);
  };

  QStringList preprocessorBlock;

  ShaderCompiler();

  static gl::Program compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static void compileProgramFromFiles_SaveBinary(const QString& targetBinaryFile, const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  static void compileProgramFromFiles_SaveBinary(const CompileSettings& settings);
  static void compileProgramFromFiles_SaveBinary_SubProcess(const CompileSettings& settings);

  static void startCompileProcess();
  static void keepCompileProcessAlive();
  static void endCompileProcess();

private:
  static QProcess compileProcess;

  bool compile(gl::ShaderObject* shaderObject, const QDir& shaderDir);
  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static const QMap<QString, gl::ShaderType>& shaderTypes();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERCOMPILER_H
