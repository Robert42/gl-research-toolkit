#include <glrt/system.h>
#include <glrt/scene/fps-debug-controller.h>

#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/renderer/bvh-usage.h>


#include <assimp/version.h>

#include <nvcommandlist.h>


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


#ifdef Q_OS_LINUX
#include <GL/glx.h>
#endif



namespace glrt {

glm::ivec2 System::_windowSize;
glm::ivec3 System::maxComputeWorkGroupCount = glm::ivec3(-1);
glm::ivec3 System::maxComputeWorkGroupSize = glm::ivec3(-1);
int System::maxComputeWorkGroupInvocations = -1;

System::System(int& argc, char** argv, const Settings& settings)
  : application(argc, argv),
  _fullscreen(!settings.windowedMode)
{
  initSplashscreen(settings);

  shaderCompiler = new renderer::ShaderCompiler(!settings.onlyOpenGLContext);

  // make glm print a vec3 as [1.2, 0.1, 0.0] instead of [1,2, 0,1, 0,0]
  std::locale::global(std::locale("C"));

  renderer::init_bvh_shader_macros();

  initSDL(settings);
  initGLEW(settings);
  initNVCommandlist(settings);

  verifyGLFeatures();
  verifyNVidiaFeatures();
  verifyAssimpVersion();
}


System::~System()
{
  delete shaderCompiler;

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

  if(_fullscreen)
  {
#ifdef QT_DEBUG
    SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN);
#endif

    SDL_DisplayMode bestDisplayMode;

    int best_display_index = -1;
    int best_size_diff = std::numeric_limits<int>::max();
    int best_refresh_rate = 0;
    quint32 best_format_bits_per_pixel = 0;
    quint32 best_format_bytes_per_pixel = 0;

    Q_UNUSED(best_format_bits_per_pixel);
    Q_UNUSED(best_format_bytes_per_pixel);


    for(int display_index = 0; display_index<SDL_GetNumVideoDisplays(); ++display_index)
    {
      int num_display_modes = SDL_GetNumDisplayModes(display_index);
      for(int i=0; i<num_display_modes; ++i)
      {
        SDL_DisplayMode candidate;
        CALL_SDL_CRITICAL(0==SDL_GetDisplayMode(display_index, i, &candidate));

        int size_diff = glm::abs(candidate.w-_windowSize.x) + glm::abs(candidate.h-_windowSize.y);
        if(size_diff > best_size_diff)
          continue;

        if(SDL_ISPIXELFORMAT_INDEXED(candidate.format))
          continue;
        if(SDL_BITSPERPIXEL(candidate.format) < 24)
          continue;
        if(SDL_BYTESPERPIXEL(candidate.format) < 3)
          continue;

        if(candidate.refresh_rate < best_refresh_rate)
          continue;

        bestDisplayMode = candidate;
        best_display_index = display_index;
        best_refresh_rate = candidate.refresh_rate;
        best_format_bits_per_pixel = SDL_BITSPERPIXEL(candidate.format);
        best_format_bytes_per_pixel = SDL_BYTESPERPIXEL(candidate.format);
        best_size_diff = size_diff;
      }
    }

    SDL_DisplayMode usedDisplayMode;
    CALL_SDL_CRITICAL(SDL_GetClosestDisplayMode(best_display_index, &bestDisplayMode, &usedDisplayMode));
    CALL_SDL_CRITICAL(0==SDL_SetWindowDisplayMode(sdlWindow, &usedDisplayMode));

  }
}

void System::initSplashscreen(const Settings& settings)
{
  if(Q_UNLIKELY(settings.onlyOpenGLContext))
    return;

  SplashscreenStyle* splashscreenStyle = settings.splashscreenStyle;

  if(splashscreenStyle == nullptr)
    splashscreenStyle = SplashscreenStyle::createFallbackStyle(settings.windowTitle);

  Q_ASSERT(splashscreen == nullptr);
  splashscreen = splashscreenStyle->createQSplashScreen(true);
}

void System::initSDL(const Settings& settings)
{
  SPLASHSCREEN_MESSAGE("init SDL");

  CALL_SDL_CRITICAL(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) == 0);

  uint32_t additionalFlags = 0;

  if(_fullscreen)
    additionalFlags |= SDL_WINDOW_BORDERLESS;

  sdlWindow = SDL_CreateWindow(settings.windowTitle.toUtf8().data(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               settings.windowSize.x,
                               settings.windowSize.y,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | additionalFlags);
  CALL_SDL_CRITICAL(sdlWindow != nullptr);

  _windowSize =  settings.windowSize;

  sdlGlContext = SDL_GL_CreateContext(sdlWindow);
  CALL_SDL_CRITICAL(sdlGlContext != nullptr);

#ifdef QT_DEBUG
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

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
  SPLASHSCREEN_MESSAGE("init GLEW");

  GLenum error =  glewInit();
  if(error != GLEW_OK)
    throw GLRT_EXCEPTION(QString("Initializing glew failed!\nError: %0").arg(reinterpret_cast<const char*>(glewGetErrorString(error))));
  if(!glewIsSupported(QString("GL_VERSION_%0_%1").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str()))
    throw GLRT_EXCEPTION(QString("The requested OpenGL version %0.%1 is not supported! => Aborting!").arg(settings.openglVersionMajor()).arg(settings.openglVersionMinor()).toStdString().c_str());
}

inline GLint print_gl_integer(GLenum variable, const char* variableName)
{
  GLint value = -1;
  glGetIntegerv(variable, &value);
  qDebug() << "OpenGL Variable: " << variableName << " = " << value;
  return value;
}

inline glm::ivec3 print_gl_integer_vec3(GLenum variable, const char* variableName)
{
  glm::ivec3 value(-1);
  for(int i=0; i<3; ++i)
    glGetIntegeri_v(variable, GLuint(i), &value[i]);
  qDebug() << "OpenGL Variable: " << variableName << " = " << value;
  return value;
}

NVCPROC myglGetProcAddress(const char* name)
{
#ifdef Q_OS_LINUX
  return reinterpret_cast<NVCPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)));
#endif
}

void System::initNVCommandlist(const Settings& settings)
{
  Q_UNUSED(settings);

  init_NV_command_list(myglGetProcAddress);
}

#define PRINT_GL_INTEGER(x) print_gl_integer(x, #x)
#define PRINT_GL_INTEGER_VEC3(x) print_gl_integer_vec3(x, #x)

void System::verifyGLFeatures()
{
  SPLASHSCREEN_MESSAGE("verify OpenGL features");

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

  maxComputeWorkGroupCount = PRINT_GL_INTEGER_VEC3(GL_MAX_COMPUTE_WORK_GROUP_COUNT);
  maxComputeWorkGroupSize = PRINT_GL_INTEGER_VEC3(GL_MAX_COMPUTE_WORK_GROUP_SIZE);
  maxComputeWorkGroupInvocations = PRINT_GL_INTEGER(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);

  Q_UNUSED(suppressLog);
}

void System::verifyNVidiaFeatures()
{
  SPLASHSCREEN_MESSAGE("verify NVidia OpenGL Extension");

  // See http://blog.icare3d.org/ for a more complete list of interesting Features

  // http://developer.download.nvidia.com/opengl/specs/GL_NV_gpu_shader5.txt
    if(!GLEW_NV_gpu_shader5)
      throw GLRT_EXCEPTION(QString("Missing opengl extension NV_gpu_shader5"));

  // https://www.opengl.org/registry/specs/NV/bindless_texture.txt
  if(!GLEW_NV_bindless_texture)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_bindless_texture"));

  if(!GLEW_EXT_texture_filter_anisotropic)
    throw GLRT_EXCEPTION(QString("Missing opengl extension EXT_texture_filter_anisotropic"));

  // http://developer.download.nvidia.com/opengl/specs/GL_NV_command_list.txt
  // http://on-demand.gputechconf.com/gtc/2015/presentation/S5135-Christoph-Kubisch-Pierre-Boudier.pdf
  if(glDrawCommandsNV == nullptr)
    throw GLRT_EXCEPTION(QString("Missing opengl extension NV_command_list"));
}

void System::verifyAssimpVersion()
{
  const unsigned int expectedMajorVersion = 3;
  const unsigned int expectedMinorVersion = 2;

  const unsigned int expectedVersion = expectedMajorVersion*1000 + expectedMinorVersion;
  const unsigned int aiVersion = aiGetVersionMajor()*1000 + aiGetVersionMinor();

  Q_ASSERT(expectedVersion<=aiVersion);
}


} // namespace renderer
