#include <glrt/application.h>
#include <glrt/debug-gui.h>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv);

  while(app.isRunning())
  {
    sf::Event event;
    while(app.pollEvent(event))
    {
    }

    app.update();

    app.beginDraw();
    app.endDraw();
  }

  return 0;
}
