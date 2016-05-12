#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include <QStringList>
#include <QTcpSocket>

#include <glrt/renderer/toolkit/shader-compiler.h>


class Compiler final : public QObject
{
  Q_OBJECT
public:
  Compiler();
  ~Compiler();

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  QTcpSocket tcpSocket;

private slots:
  void compile();
  void disconnected();

private:
  bool dialogVisible = false;

  void sendCompiledProgram(const QByteArray& byteArray);
  void sendData(glrt::TcpMessages::Id id, const QByteArray& byteArray);

  void shaderDialogVisible(glrt::renderer::ShaderCompiler::DialogAction action);
};


#endif // COMPILER_H
