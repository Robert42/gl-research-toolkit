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

Window::Window(int argc, char** argv)
  : window(sf::VideoMode(1024, 768),
           "Title",
           sf::Style::Default,
           sf::ContextSettings(32, 32, 0, 4, 5))
{
}

} // namespace glrt

