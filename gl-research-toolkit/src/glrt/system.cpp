#include <glrt/system.h>

/*! \namespace glrt
\ingroup glrt
\brief The namespace of the OpenGL Research Toolkit
*/

/*! \class glrt::System
\ingroup glrt
\inheaderfile glrt/system.h
\brief The main class of this toolkit. Initializes all dependencies.

\note This is the only class mandatory for this toolkit.
      If you use the Application helper class, it already automatically creates an instance of this class for you.
*/


namespace glrt {

System::System(int argc, char** argv, const Settings& settings)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  CALL_SDL_CRITICAL(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) == 0);

  sdlWindow = SDL_CreateWindow(settings.windowTitle.toUtf8().data(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               settings.windowSize.x,
                               settings.windowSize.y,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  CALL_SDL_CRITICAL(sdlWindow != nullptr);

  sdlGlContext = SDL_GL_CreateContext(sdlWindow);

  SDL_ShowWindow(sdlWindow);

  GLenum error =  glewInit();

  if(error != GLEW_OK)
    qCritical() << "Initializing glew failed!\nError: " << glewGetErrorString(error);
  if(!glewIsSupported(QString("GL_VERSION_%0_%1").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str()))
    qCritical() << QString("The requested OpenGL version %0.%1 is not supported! => Aborting!").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str();
}


System::~System()
{
  SDL_GL_DeleteContext(sdlGlContext);
  SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}



} // namespace glrt

