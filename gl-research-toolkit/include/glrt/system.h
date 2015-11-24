#ifndef GLRT_WINDOW_H
#define GLRT_WINDOW_H

#include "dependencies.h"


namespace glrt {

class System
{
public:
  struct Settings
  {
    QString title = "Hello World :)";
    int minOpenglVersion = 450;
    SDL_DisplayMode displayMode;

    static Settings simpleWindow(const QString& title="Hello World :)", const glm::ivec2 resolution = glm::ivec2(1024, 768))
    {
      Settings settings;
      settings.title = title;
      settings.displayMode.w = resolution.x;
      settings.displayMode.h = resolution.y;
      return settings;
    }

    int openglVersionMajor() const{return minOpenglVersion/100;}
    int openglVersionMinor() const{return (minOpenglVersion/10)%10;}

  private:
    Settings()
    {
      displayMode.driverdata = nullptr;
      displayMode.w = 0;
      displayMode.h = 0;
      displayMode.refresh_rate = 0;
    }
  };

  SDL_Window* sdlWindow;
  SDL_GLContext sdlGlContext;

  System(int argc, char** argv, const Settings& settings = Settings::simpleWindow());
  ~System();
};

} // namespace glrt

#endif // GLRT_WINDOW_H
