#include "compiler.h"

#include <iostream>

#include <QCoreApplication>

#include <glrt/renderer/toolkit/shader-compiler.h>

#include "debugmessage.h"

typedef glrt::renderer::ShaderCompiler ShaderCompiler;

extern bool isRunning;

Compiler::Compiler()
{
  tcpSocket.connectToHost(QHostAddress::LocalHost, GLRT_SHADER_COMPILER_PORT);
  connect(&tcpSocket, &QTcpSocket::readyRead, this, &Compiler::compile, Qt::QueuedConnection);
  connect(&tcpSocket, &QTcpSocket::disconnected, this, &Compiler::disconnected);
}

Compiler::~Compiler()
{
  tcpSocket.disconnectFromHost();
}

void Compiler::timerEvent(QTimerEvent*)
{
  compile();
}


void Compiler::compile()
{
  glrt::TcpMessages messages;
  messages.connection = &tcpSocket;

  if(isCurrentlyDialogShown())
    return;

  while(messages.waitForReadyRead(250))
  {
    glrt::TcpMessages::Message msg = messages.readMessage();

    if(msg.id ==  ShaderCompiler::shaderCompileCommand)
    {
      ShaderCompiler::CompileSettings settings = ShaderCompiler::CompileSettings::fromString(QString::fromUtf8(msg.byteArray));
      sendCompiledProgram(ShaderCompiler::singleton().compileProgramFromFiles_GetBinary(settings));
      break;
    }else
    {
      debugMessage("Unknown Command", QString("Unknown Command %0").arg(quint32(msg.id)));
      qWarning() << "Unknown Command" << quint32(msg.id);
      std::exit(0);
    }
  }
}

void Compiler::disconnected()
{
  debugMessage("Quit!","RECEIVED QUIT");
  qApp->quit();
  isRunning = false;
}

void Compiler::sendCompiledProgram(const QByteArray& byteArray)
{
  sendData(ShaderCompiler::glslBytecode, byteArray);
}

void Compiler::sendData(glrt::TcpMessages::Id id, const QByteArray& byteArray)
{
  glrt::TcpMessages messages;
  messages.connection = &tcpSocket;

  glrt::TcpMessages::Message msg;
  msg.id = id;
  msg.byteArray = byteArray;

  messages.sendMessage(msg);
}
