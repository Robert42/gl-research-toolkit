#include <glrt/application.h>

#include "compiler.h"

#include "debugmessage.h"

bool isRunning = true;

int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::needOnlyOpenGLContext());

  Compiler compiler;
  Q_UNUSED(compiler);

  int result = qApp->exec();

  while(isRunning)
    qApp->processEvents(QEventLoop::WaitForMoreEvents);

  debugMessage("main()", "exit");

  return result;
}
