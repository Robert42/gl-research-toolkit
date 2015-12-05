#include <glrt/system.h>
#include <glrt/toolkit/temp-shader-file.h>


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

  initSDL(settings);
  initGLEW(settings);

  verifyNVidiaFeatures();

  TempShaderFile::init();
}


System::~System()
{
  SDL_GL_DeleteContext(sdlGlContext);
  SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}


void System::initSDL(const Settings& settings)
{
  CALL_SDL_CRITICAL(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) == 0);

  sdlWindow = SDL_CreateWindow(settings.windowTitle.toUtf8().data(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               settings.windowSize.x,
                               settings.windowSize.y,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  CALL_SDL_CRITICAL(sdlWindow != nullptr);

  sdlGlContext = SDL_GL_CreateContext(sdlWindow);
  CALL_SDL_CRITICAL(sdlGlContext != nullptr);

  if(settings.VSync)
  {
    if(SDL_GL_SetSwapInterval(-1) < 0)
      CALL_SDL(SDL_GL_SetSwapInterval(1)  == 0);
  }else
  {
    CALL_SDL(SDL_GL_SetSwapInterval(0)  == 0);
  }


  SDL_ShowWindow(sdlWindow);
}


void System::initGLEW(const Settings& settings)
{
  GLenum error =  glewInit();
  if(error != GLEW_OK)
    throw GLRT_EXCEPTION(QString("Initializing glew failed!\nError: %0").arg(reinterpret_cast<const char*>(glewGetErrorString(error))));
  if(!glewIsSupported(QString("GL_VERSION_%0_%1").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str()))
    throw GLRT_EXCEPTION(QString("The requested OpenGL version %0.%1 is not supported! => Aborting!").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str());
}

void System::verifyNVidiaFeatures()
{
  // See http://blog.icare3d.org/ for a more complete list of interesting Features

  // https://developer.nvidia.com/sites/default/files/akamai/opengl/specs/GL_NV_fill_rectangle.txt
  if(!GLEW_NV_fill_rectangle)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_fill_rectangle"));

  // https://www.opengl.org/registry/specs/NV/bindless_texture.txt
  if(!GLEW_ARB_bindless_texture)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_bindless_texture"));
}


} // namespace glrt

