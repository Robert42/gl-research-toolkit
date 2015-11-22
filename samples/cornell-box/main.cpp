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

    app.window.clear(sf::Color::Black);


    app.window.display();
  }

  return 0;
}
