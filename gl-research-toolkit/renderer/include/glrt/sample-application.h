#ifndef GLRT_SAMPLEAPPLICATION_H
#define GLRT_SAMPLEAPPLICATION_H

#include <glrt/application.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/renderer/sample-resource-manager.h>
#include <glrt/renderer/forward-renderer.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/gl.hpp>

namespace glrt {

class SampleApplication : public glrt::Application
{
public:
  struct Settings final
  {
    Uuid<scene::Scene> sceneToLoad = glrt::renderer::SampleResourceManager::sponzaScene();

    static Settings techDemo()
    {
      return Settings();
    }
  };

  glrt::renderer::SampleResourceManager resourceManager;
  glrt::scene::Scene scene;

  glrt::renderer::ForwardRenderer renderer;
  glrt::renderer::debugging::ShaderDebugPrinter shaderDebugPrinter;

  glrt::gui::AntTweakBar antweakbar;

  SampleApplication(int& argc, char** argv,
                    const gui::AntTweakBar::Settings& guiSettings,
                    const SampleApplication::Settings& sampleApplicationSettings=SampleApplication::Settings::techDemo(),
                    const Application::Settings& applicationSettings=Application::Settings::techDemo(),
                    const System::Settings& systemSettings = System::Settings::simpleWindow());

  bool handleEvents(const SDL_Event& event);

  float update();

  void beginDrawing();
  void drawScene();
  void endDrawing();

public:
  void initGui();
};

} // namespace glrt

#endif // GLRT_RENDERER_SAMPLEAPPLICATION_H
