#include <glrt/application.h>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv);

  while(app.window.isOpen())
  {
    sf::Event event;
    while(app.pollEvent(event))
    {
    }

    app.update(1.f);

    app.window.clear(sf::Color::Black);

    app.window.pushGLStates();
    app.window.resetGLStates();
    app.sfgui.Display(app.window);
    app.window.popGLStates();

    app.window.display();
  }

  return 0;
}
