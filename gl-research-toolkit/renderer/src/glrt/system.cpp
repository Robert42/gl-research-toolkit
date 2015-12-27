#include <glrt/system.h>
#include <glrt/renderer/toolkit/temp-shader-file.h>
#include <glrt/scene/debug-camera.h>

#include <glrt/glsl/layout-constants.h>


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

System::System(int& argc, char** argv, const Settings& settings)
  : application(argc, argv)
{
  initSplashscreen(settings);

  // make glm print a vec3 as [1.2, 0.1, 0.0] instead of [1,2, 0,1, 0,0]
  std::locale::global(std::locale("C"));

  initSDL(settings);
  initGLEW(settings);

  verifyGLFeatures();
  verifyNVidiaFeatures();

  renderer::TempShaderFile::init();
}


System::~System()
{
  SDL_GL_DeleteContext(sdlGlContext);
  SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}


void System::showWindow()
{
  Q_ASSERT(splashscreen != nullptr);
  splashscreen->finish(nullptr);
  delete splashscreen;
  splashscreen = nullptr;

  SDL_ShowWindow(sdlWindow);
}

void System::initSplashscreen(const Settings& settings)
{
  SplashscreenStyle* splashscreenStyle = settings.splashscreenStyle;

  if(splashscreenStyle == nullptr)
    splashscreenStyle = SplashscreenStyle::createFallbackStyle(settings.windowTitle);

  Q_ASSERT(splashscreen == nullptr);
  splashscreen = splashscreenStyle->createQSplashScreen(true);
}

void System::initSDL(const Settings& settings)
{
  showSplashscreenMessage("init SDL");

  CALL_SDL_CRITICAL(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) == 0);

  sdlWindow = SDL_CreateWindow(settings.windowTitle.toUtf8().data(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               settings.windowSize.x,
                               settings.windowSize.y,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  CALL_SDL_CRITICAL(sdlWindow != nullptr);

  DebugCamera::windowSize = settings.windowSize;

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
}


void System::initGLEW(const Settings& settings)
{
  showSplashscreenMessage("init GLEW");

  GLenum error =  glewInit();
  if(error != GLEW_OK)
    throw GLRT_EXCEPTION(QString("Initializing glew failed!\nError: %0").arg(reinterpret_cast<const char*>(glewGetErrorString(error))));
  if(!glewIsSupported(QString("GL_VERSION_%0_%1").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str()))
    throw GLRT_EXCEPTION(QString("The requested OpenGL version %0.%1 is not supported! => Aborting!").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str());
}

inline GLint print_gl_integer(GLenum variable, const char* variableName)
{
  GLint value;
  glGetIntegerv(variable, &value);
  qDebug() << "OpenGL Capability: " << variableName << " = " << value;
  return value;
}

#define PRINT_GL_INTEGER(x) print_gl_integer(x, #x)

void System::verifyGLFeatures()
{
  showSplashscreenMessage("verify OpenGL features");

  Logger::SuppressDebug suppressLog;

  // If a system doesn't support one of there numbers

  if(PRINT_GL_INTEGER(GL_MAX_VERTEX_ATTRIB_BINDINGS) < EXPECTED_GL_MAX_VERTEX_ATTRIB_BINDINGS)
    throw GLRT_EXCEPTION(QString("Unsupported number of opengl vertex attribute bindings."));

  if(PRINT_GL_INTEGER(GL_MAX_FRAGMENT_ATOMIC_COUNTERS) < 2)
    throw GLRT_EXCEPTION(QString("Unsupported number of opengl vertex attribute bindings."));

  if(PRINT_GL_INTEGER(GL_MAX_COMBINED_UNIFORM_BLOCKS) < EXPECTED_GL_MAX_COMBINED_UNIFORM_BLOCKS)
    throw GLRT_EXCEPTION(QString("Unsupported number of opengl uniform blocks."));
  if(PRINT_GL_INTEGER(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS) < EXPECTED_GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS)
    throw GLRT_EXCEPTION(QString("Unsupported number of opengl shader storage buffers."));

  Q_UNUSED(suppressLog);
}

void System::verifyNVidiaFeatures()
{
  showSplashscreenMessage("verify NVidia OpenGL Extension");

  // See http://blog.icare3d.org/ for a more complete list of interesting Features

  // https://developer.nvidia.com/sites/default/files/akamai/opengl/specs/GL_NV_fill_rectangle.txt
  if(!GLEW_NV_fill_rectangle)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_fill_rectangle"));

  // https://developer.nvidia.com/sites/default/files/akamai/opengl/specs/GL_NV_fragment_shader_interlock.txt
  if(!GLEW_NV_fragment_shader_interlock)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_fragment_shader_interlock"));

  // https://www.opengl.org/registry/specs/NV/bindless_texture.txt
  if(!GLEW_ARB_bindless_texture)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_bindless_texture"));
}


} // namespace renderer
