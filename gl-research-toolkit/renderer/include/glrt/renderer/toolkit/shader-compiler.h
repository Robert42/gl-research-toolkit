#ifndef GLRT_RENDERER_SHADERCOMPILER_H
#define GLRT_RENDERER_SHADERCOMPILER_H

#include <glrt/dependencies.h>

#include <glrt/renderer/gl/program.h>
#include <glrt/renderer/gl/shader-type.h>
#include <glrt/toolkit/tcp-messages.h>

#include <QProcess>
#include <QTimer>
#include <QTcpServer>

namespace glrt {
namespace renderer {

class ShaderCompiler : public QObject
{
  Q_OBJECT
public:
  static const TcpMessages::Id shaderCompileCommand = TcpMessages::Id(0x1);

  static const TcpMessages::Id glslBytecode = TcpMessages::Id(0x100);
  static const TcpMessages::Id startedWaitingForUserInput = TcpMessages::Id(0x101);
  static const TcpMessages::Id finishedWaitingForUserInput = TcpMessages::Id(0x102);

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

  ShaderCompiler(bool startServer);
  ~ShaderCompiler();

  static ShaderCompiler& singleton();

  gl::Program compileProgramFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  void compileProgramFromFiles_SaveBinary(const QString& targetBinaryFile, const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());
  void compileProgramFromFiles_SaveBinary(const CompileSettings& settings);
  void compileProgramFromFiles_SaveBinary_SubProcess(const CompileSettings& settings);

private:
  static ShaderCompiler* _singleton;

  int nProcessStarted = 0;
  QProcess compileProcess;
  QTcpServer tcpServer;
  QTcpSocket* tcpConnection = nullptr;

  bool compile(gl::ShaderObject* shaderObject, const QDir& shaderDir, const QStringList& preprocessorBlock);
  static gl::ShaderObject createShaderFromFiles(const QString& name, const QDir& shaderDir, const QStringList& preprocessorBlock=QStringList());

  static const QMap<QString, gl::ShaderType>& shaderTypes();

  bool compilerProcessIsRunning() const;

private slots:
  void startCompileProcess();
  void endCompileProcess();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SHADERCOMPILER_H
