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

} // namespace glrt

