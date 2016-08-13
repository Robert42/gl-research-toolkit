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

  void compile(const QString& shaderFile);

private:
  void sendCompiledProgram(const QByteArray& byteArray);
  void waitForReceived();
};


#endif // COMPILER_H
