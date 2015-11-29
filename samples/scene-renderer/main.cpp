#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/scene.h>

#include <glhelper/gl.hpp>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Scene-Renderer"));

  glrt::scene::Scene scene;

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    // T O D O : Add your drawing code here

    app.swapWindow();
  }

  return 0;
}
