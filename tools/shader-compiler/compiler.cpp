#include "compiler.h"

#include <iostream>

#include <QCoreApplication>
#include <QMessageBox>

#include <glrt/renderer/toolkit/shader-compiler.h>

typedef glrt::renderer::ShaderCompiler ShaderCompiler;

Compiler::Compiler()
{
}

Compiler::~Compiler()
{
  ShaderCompiler::shaderDialogVisible = [](ShaderCompiler::DialogAction){};
}

void Compiler::compile(const QString& shaderFile)
{
  ShaderCompiler::CompileSettings settings = ShaderCompiler::CompileSettings::fromString(shaderFile);
  QByteArray byteCode = ShaderCompiler::singleton().compileProgramFromFiles_GetBinary(settings);
  sendCompiledProgram(byteCode);

  waitForReceived();
}

void Compiler::sendCompiledProgram(const QByteArray& byteArray)
{
  std::cout.write(byteArray.data(), byteArray.length());
  std::cerr << "[Finished Compiling]" << std::endl;
}

void Compiler::waitForReceived()
{
  std::string response;
  std::cin >> response;
  if(response != "[Received]")
    QMessageBox::warning(nullptr, "Shader-Compile Process", "Didn't get a response!");
}
