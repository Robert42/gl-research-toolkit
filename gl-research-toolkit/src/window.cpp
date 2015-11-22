#include <glrt/window.h>

/*! \namespace glrt
\ingroup glrt
\brief The namespace of the OpenGL Research Toolkit
*/

/*! \class glrt::Window
\ingroup glrt
\inheaderfile glrt/window.h
\brief A wrapper class creating the OpenGL Window and handling basic events.

*/

namespace glrt {

Window::Window(int argc, char** argv, const Settings& settings)
  : window(settings.videoMode,
           settings.title,
           settings.style,
           settings.contextSettings)
{
}


bool Window::pollEvent(sf::Event& event)
{
  while(window.pollEvent(event))
  {
    switch(event.type)
    {
    case sf::Event::Closed:
      window.close();
      break;
    case sf::Event::KeyPressed:
      switch(event.key.code)
      {
      case sf::Keyboard::Escape:
        window.close();
        continue;
      }
      break;
    }

    return true;
  }

  return false;
}


} // namespace glrt

