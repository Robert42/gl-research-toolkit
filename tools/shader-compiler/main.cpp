#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/renderer/toolkit/shader-compiler.h>

#include <glhelper/gl.hpp>

#include <QTextStream>

typedef glrt::renderer::ShaderCompiler ShaderCompiler;

int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Shader-Compiler"));

  if(argc != 2)
    throw glrt::GLRT_EXCEPTION("Expected exactly one attribute");
  QString settingsArgument = argv[1];

  ShaderCompiler::CompileSettings settings = ShaderCompiler::CompileSettings::fromString(settingsArgument);

  ShaderCompiler::compileProgramFromFiles_SaveBinary(settings);
/*
  app.showWindow();

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
      // T O D O : Add your event handling code here
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    // T O D O : Add your drawing code here

    app.swapWindow();
  }

*/
  return 0;
}
