#include <glrt/application.h>
#include <glrt/debug-gui.h>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv);
  glrt::DebugGui debugGui;

  while(app.window.isOpen())
  {
    sf::Event event;
    while(app.pollEvent(event))
    {
      debugGui.handleEvents(event);
    }

    app.update(1.f);

    app.window.clear(sf::Color::Black);

    app.window.pushGLStates();
    app.window.resetGLStates();
    debugGui.draw(app.window);
    app.window.popGLStates();

    app.window.display();
  }

  return 0;
}
