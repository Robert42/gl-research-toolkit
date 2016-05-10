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

  ShaderCompiler();
  ~ShaderCompiler();

  static ShaderCompiler& singleton();

  gl::Program compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  void compileProgramFromFiles_SaveBinary(const QString& targetBinaryFile, const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  void compileProgramFromFiles_SaveBinary(const CompileSettings& settings);
  void compileProgramFromFiles_SaveBinary_SubProcess(const CompileSettings& settings);

private:
  static ShaderCompiler* _singleton;

  QProcess compileProcess;
  QTcpServer tcpServer;
  QTimer compileProcessAliveTimer;

  bool compile(gl::ShaderObject* shaderObject, const QDir& shaderDir, const QStringList& preprocessorBlock);
  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static const QMap<QString, gl::ShaderType>& shaderTypes();

private slots:
  void startCompileProcess();
  void keepCompileProcessAlive();
  void endCompileProcess();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERCOMPILER_H
