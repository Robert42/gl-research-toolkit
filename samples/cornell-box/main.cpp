#include <glrt/application.h>
#include <glrt/debug-gui.h>


int main(int argc, char** argv)
{
  sf::Clock clock;

  glrt::Application app(argc, argv);
  glrt::DebugGui debugGui;

  while(app.window.isOpen())
  {
    sf::Event event;
    while(app.pollEvent(event))
    {
      debugGui.handleEvents(event);
    }

    float deltaTime = clock.restart().asSeconds();

    app.update(deltaTime);
    debugGui.update(deltaTime);

    app.window.clear(sf::Color::Black);

    debugGui.draw(app.window);

    app.window.display();
  }

  return 0;
}
