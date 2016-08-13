#include <glrt/system.h>
#include <QMessageBox>

#include "compiler.h"

int main(int argc, char** argv)
{
  glrt::System system(argc, argv, glrt::System::Settings::needOnlyOpenGLContext());
  glrt::renderer::ShaderCompiler::registerGlrtShaderIncludeDirectories();

  Compiler compiler;

  if(qApp->arguments().length() != 2)
  {
    QMessageBox::warning(nullptr, "Shader-Compile Process", "The Shader-Compile process expects exactly one argument being the path to the shader source file!");
    return -1;
  }

  compiler.compile(qApp->arguments().last());

  Q_UNUSED(system);
  Q_UNUSED(compiler);
  return 0;
}
