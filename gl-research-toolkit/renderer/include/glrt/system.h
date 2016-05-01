#ifndef GLRT_SYSTEM_H
#define GLRT_SYSTEM_H

#include <glrt/renderer/dependencies.h>
#include <glrt/toolkit/logger.h>
#include <glrt/splashscreen-style.h>

#include <QApplication>
#include <QSplashScreen>

namespace glrt {

class System final
{
public:
  struct Settings final
  {
    QString windowTitle = "Hello World :)";
    glm::ivec2 windowSize = glm::ivec2(640, 480);
    SplashscreenStyle* splashscreenStyle = nullptr;
    int minOpenglVersion = 450;
    bool VSync = true;

    static Settings simpleWindow(const QString& windowTitle="Hello World :)", const glm::ivec2 windowSize = glm::ivec2(1024, 768))
    {
      Settings settings;
      settings.windowTitle = windowTitle;
      settings.windowSize = windowSize;
      return settings;
    }

    static Settings addVSync(Settings settings)
    {
      settings.VSync = true;
      return settings;
    }

    int openglVersionMajor() const{return minOpenglVersion/100;}
    int openglVersionMinor() const{return (minOpenglVersion/10)%10;}

  private:
    Settings()
    {
    }
  };

  static GLint maxComputeWorkGroupCount;

  SDL_Window* sdlWindow;
  SDL_GLContext sdlGlContext;

  System(int& argc, char** argv, const Settings& settings = Settings::simpleWindow());
  ~System();

  void showWindow();

  static glm::ivec2 windowSize(){return _windowSize;}
  static float windowAspectRatio(){return float(_windowSize.x)/float(_windowSize.y);}

private:
  QApplication application;
  QSplashScreen* splashscreen = nullptr;
  Logger logger;

  static glm::ivec2 _windowSize;

  void initSplashscreen(const Settings& settings);
  void initSDL(const Settings& settings);
  void initGLEW(const Settings& settings);
  void initNVCommandlist(const Settings& settings);
  void verifyGLFeatures();
  void verifyNVidiaFeatures();
  void verifyAssimpVersion();
};

} // namespace glrt

#endif // GLRT_SYSTEM_H
