#include <glrt/application.h>

/*! \namespace glrt
\ingroup glrt
\brief The namespace of the OpenGL Research Toolkit
*/

/*! \class glrt::Application
\ingroup glrt
\inheaderfile glrt/application.h
\brief The main calss of this toolkit. Initializes all dependencies.

*/

namespace glrt {

Application::Application(int argc, char** argv, const Settings& settings)
  : window(settings.videoMode,
           settings.title,
           settings.style,
           settings.contextSettings)
{
}


bool Application::pollEvent(sf::Event& event)
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

