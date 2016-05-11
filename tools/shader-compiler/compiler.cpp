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
  currentCommand += QString::fromUtf8(tcpSocket.readAll());
  //debugMessage("currentCommand", currentCommand);

  receivedCommands << currentCommand.split('\n', QString::KeepEmptyParts);
  currentCommand = receivedCommands.last();
  receivedCommands.removeLast();

  if(isCurrentlyDialogShown())
    return;

  bool handlingCommands = !receivedCommands.isEmpty();
  while(handlingCommands)
  {
    debugMessage("receivedCommands", "\""+receivedCommands.join("\"\n\"")+"\"");

    ShaderCompiler::CompileSettings settings;
    if(ShaderCompiler::CompileSettings::fromStringList(settings, receivedCommands))
    {
      debugMessage("compiling"
                   "", settings.toString());
      ShaderCompiler::singleton().compileProgramFromFiles_SaveBinary(settings);
      debugMessage("compiled", settings.toString());
    }else
    {
      handlingCommands = false;
    }
  }
}

void Compiler::disconnected()
{
  debugMessage("Quit!","RECEIVED QUIT");
  qApp->quit();
  isRunning = false;
}
