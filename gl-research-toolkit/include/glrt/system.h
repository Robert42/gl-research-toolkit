#ifndef GLRT_WINDOW_H
#define GLRT_WINDOW_H

#include "dependencies.h"


namespace glrt {

class System
{
public:
  struct Settings
  {
    QString windowTitle = "Hello World :)";
    glm::ivec2 windowSize = glm::ivec2(640, 480);
    int minOpenglVersion = 450;

    static Settings simpleWindow(const QString& windowTitle="Hello World :)", const glm::ivec2 windowSize = glm::ivec2(1024, 768))
    {
      Settings settings;
      settings.windowTitle = windowTitle;
      settings.windowSize = windowSize;
      return settings;
    }

    int openglVersionMajor() const{return minOpenglVersion/100;}
    int openglVersionMinor() const{return (minOpenglVersion/10)%10;}

  private:
    Settings()
    {
    }
  };

  SDL_Window* sdlWindow;
  SDL_GLContext sdlGlContext;

  System(int argc, char** argv, const Settings& settings = Settings::simpleWindow());
  ~System();

private:
  void initSDL(const Settings& settings);
  void initGLEW(const Settings& settings);
};

} // namespace glrt

#endif // GLRT_WINDOW_H
