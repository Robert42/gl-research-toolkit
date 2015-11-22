#include <glrt/window.h>


int main(int argc, char** argv)
{
  glrt::Window renderWindow(argc, argv);

  while(renderWindow.window.isOpen())
  {
    sf::Event event;
    while(renderWindow.pollEvent(event))
    {
    }

    renderWindow.window.clear(sf::Color::Black);


    renderWindow.window.display();
  }

  return 0;
}
