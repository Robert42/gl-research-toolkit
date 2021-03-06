#include <glrt/application.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/gl.hpp>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("TemplateMinimum"));

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

  return 0;
}
