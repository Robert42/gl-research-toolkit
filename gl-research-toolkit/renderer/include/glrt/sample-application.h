#ifndef GLRT_SAMPLEAPPLICATION_H
#define GLRT_SAMPLEAPPLICATION_H

#include <glrt/application.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/renderer/sample-resource-manager.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>
#include <glrt/renderer/voxelized-scene.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/gl.hpp>

namespace glrt {

class SampleApplication : public glrt::Application
{
public:
  struct Settings final
  {
    Uuid<scene::Scene> sceneToLoad = glrt::renderer::SampleResourceManager::defaultScene();
    bool loadDistanceField = true;
    bool deferredRenderer = false;
    padding<uint8_t, 6> _padding;

    static Settings techDemoForward()
    {
      return Settings();
    }

    static Settings techDemoDeferred()
    {
      Settings settings;
      settings.deferredRenderer = true;
      return settings;
    }
  };

  Uuid<glrt::scene::Scene> firstSceneToLoad;

  glrt::renderer::SampleResourceManager resourceManager;
  glrt::scene::Scene scene;
  glrt::renderer::VoxelizedScene voxelizedScene;

  glrt::renderer::Renderer* renderer;
  glrt::renderer::debugging::ShaderDebugPrinter shaderDebugPrinter;

  glrt::gui::AntTweakBar antweakbar;

  SampleApplication(int& argc, char** argv,
                    const gui::AntTweakBar::Settings& guiSettings,
                    const SampleApplication::Settings& sampleApplicationSettings=SampleApplication::Settings::techDemoForward(),
                    const Application::Settings& applicationSettings=Application::Settings::techDemo(),
                    const System::Settings& systemSettings = System::Settings::simpleWindow());
  ~SampleApplication();

  bool handleEvents(const SDL_Event& event);

  float update();

  void beginDrawing();
  void drawScene();
  void endDrawing();

  float drawSingleFrame();

private:
  void initGui();

  void shaderRecompileWorkaround() override;

  void loadDistanceField();
};

} // namespace glrt

#endif // GLRT_RENDERER_SAMPLEAPPLICATION_H
