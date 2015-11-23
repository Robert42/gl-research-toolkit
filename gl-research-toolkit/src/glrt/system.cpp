#include <glrt/system.h>

#include <GL/glew.h>

/*! \namespace glrt
\ingroup glrt
\brief The namespace of the OpenGL Research Toolkit
*/

/*! \class glrt::System
\ingroup glrt
\inheaderfile glrt/application.h
\brief The main class of this toolkit. Initializes all dependencies.

\note This is the only class mandatory for this toolkit.
      If you use the Application helper class, it already automatically creates an instance of this class for you.
*/


namespace glrt {

System::System(int argc, char** argv, const Settings& settings)
  : window(settings.videoMode,
           settings.title,
           settings.style,
           settings.contextSettings)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  GLenum error =  glewInit();

  if(error != GLEW_OK)
    qCritical() << "Initializing glew failed!";
}




} // namespace glrt

