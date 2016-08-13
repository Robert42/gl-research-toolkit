#ifndef GLRT_RENDERER_SHADERCOMPILER_H
#define GLRT_RENDERER_SHADERCOMPILER_H

#include <glrt/dependencies.h>

#include <glrt/renderer/gl/program.h>
#include <glrt/renderer/gl/shader-type.h>

#include <QProcess>
#include <QTimer>
#include <QTcpServer>

namespace glrt {
namespace renderer {

class ShaderCompiler : public QObject
{
  Q_OBJECT
public:
  struct CompileSettings
  {
    QString name;
    QDir shaderDir;
    QStringList preprocessorBlock;

    bool operator==(const CompileSettings& other) const;

    QString toString() const;
    static CompileSettings fromString(const QString& encodedString);
  };

  ShaderCompiler();
  ~ShaderCompiler();

  static void registerGlrtShaderIncludeDirectories();

  static ShaderCompiler& singleton();

  gl::Program compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  gl::Program compileProgramFromFiles_WithDebugger(const QString& name, const QDir& shaderDir, QStringList preprocessorBlock=QStringList());

  QByteArray compileProgramFromFiles_GetBinary(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  QByteArray compileProgramFromFiles_GetBinary(const CompileSettings& settings);
  gl::Program compileProgramFromFiles_SubProcess(const CompileSettings& settings);

  enum class DialogAction
  {
    Show,
    Hide,
    ExitApp,
  };
  static std::function<void(DialogAction)> shaderDialogVisible;

private:
  static ShaderCompiler* _singleton;

  bool compile(gl::ShaderObject* shaderObject, const QDir& shaderDir, const QStringList& preprocessorBlock);
  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static const QMap<QString, gl::ShaderType>& shaderTypes();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERCOMPILER_H
