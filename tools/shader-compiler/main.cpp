#include <glrt/system.h>

#include "compiler.h"

#include "debugmessage.h"

bool isRunning = true;

int main(int argc, char** argv)
{
  glrt::System system(argc, argv, glrt::System::Settings::needOnlyOpenGLContext());
  glrt::renderer::ShaderCompiler::registerGlrtShaderIncludeDirectories();

  Compiler compiler;

  int result = qApp->exec();

  while(isRunning)
    qApp->processEvents(QEventLoop::WaitForMoreEvents);

  Q_UNUSED(system);
  Q_UNUSED(compiler);
  return result;
}
