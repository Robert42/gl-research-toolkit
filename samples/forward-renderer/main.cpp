#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/scene.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/renderer/sample-source-manager.h>
#include <glrt/renderer/forward-renderer.h>
#include <glrt/renderer/debugging/shader-debug-printer.h>

#include <glhelper/gl.hpp>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Forward-Renderer"));

  glrt::renderer::SampleResourceManager resourceManager;

  glrt::scene::Scene scene;
  glrt::renderer::ForwardRenderer renderer(&scene);

  glrt::gui::AntTweakBar antweakbar(&app,
                                    glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                                ));

  glrt::renderer::debugging::ShaderDebugPrinter shaderDebugPrinter;

  antweakbar.createDebugSceneBar(&renderer);
  antweakbar.createDebugShaderBar(&shaderDebugPrinter);
  antweakbar.createProfilerBar(&app.profiler);

  scene.loadFromFile(GLRT_ASSET_DIR"/common/scenes/cornell-box/cornell-box.scene");

  app.showWindow();

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
      if(scene.handleEvents(event))
        continue;
      if(antweakbar.handleEvents(event))
        continue;
      if(shaderDebugPrinter.handleEvents(event))
        continue;
    }

    const float deltaTime = app.update();
    scene.update(deltaTime);
    antweakbar.update(deltaTime);

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    shaderDebugPrinter.begin();
    renderer.render();
    shaderDebugPrinter.end();
    shaderDebugPrinter.draw();


    antweakbar.draw();

    app.swapWindow();
  }

  return 0;
}
