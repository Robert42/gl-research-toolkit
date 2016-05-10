#include "compiler.h"

#include <iostream>

#include <QCoreApplication>

#include <glrt/renderer/toolkit/shader-compiler.h>

#include "debugmessage.h"

typedef glrt::renderer::ShaderCompiler ShaderCompiler;

extern bool isRunning;

Compiler::Compiler()
{
  timerId = startTimer(40);
  stillAlive.start();
}

Compiler::~Compiler()
{
}

void Compiler::timerEvent(QTimerEvent*)
{
  compile();
}

const std::streamsize max_length = 4096;
char readData[max_length];

void Compiler::compile()
{
  killTimer(timerId);
  timerId = 0;

  debugMessage("OHA!","Hello World!");

  std::streamsize length;

  do
  {
    length = std::cin.readsome(readData, max_length);
    Q_ASSERT(length <= max_length);
    debugMessage("length", QString("%0").arg(length));

    currentCommand += QString::fromUtf8(readData, int(length));
  }while(length > 0);

  debugMessage("currentCommand", currentCommand);

  receivedCommands << currentCommand.split('\n', QString::KeepEmptyParts);
  currentCommand = receivedCommands.last();
  receivedCommands.removeLast();


  bool handlingCommands = !receivedCommands.isEmpty();
  while(handlingCommands)
  {
    debugMessage("receivedCommands", receivedCommands.join('\n'));

    ShaderCompiler::CompileSettings settings;
    if(ShaderCompiler::CompileSettings::fromStringList(settings, receivedCommands))
    {
      debugMessage("ShaderCompiler::CompileSettings::fromStringList", settings.toString());
      ShaderCompiler::singleton().compileProgramFromFiles_SaveBinary(settings);
    }else if(receivedCommands.first() == "QUIT")
    {
      debugMessage("Quit!","RECEIVED QUIT");
      isRunning = false;
    }else if(receivedCommands.first() == "ALIVE")
    {
      stillAlive.restart();
    }else
    {
      handlingCommands = false;
    }
  }

  if(stillAlive.elapsed() > 4000)
  {
    debugMessage("Quit!","Quit!!!!!");
    isRunning = false;
    return;
  }
  debugMessage("Blub!","testhaha");

  timerId = startTimer(40);
}
