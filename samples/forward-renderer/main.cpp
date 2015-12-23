#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/forward-renderer.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/debugging/shader-debug-printer.h>

#include <angelscript-integration/call-script.h>

#include <glhelper/gl.hpp>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Forward-Renderer"));

  glrt::scene::Scene scene(app.sdlWindow);
  glrt::scene::ForwardRenderer renderer(&scene);

  glrt::gui::AntTweakBar antweakbar(&app,
                                    glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                                ));

  glrt::debugging::ShaderDebugPrinter shaderDebugPrinter;

  antweakbar.createDebugSceneBar(&renderer);
  antweakbar.createDebugShaderBar(&shaderDebugPrinter);
  antweakbar.createProfilerBar(&app.profiler);

  scene.loadFromFile(GLRT_ASSET_DIR"/common/scenes/cornell-box/cornell-box.scene");

  AngelScriptIntegration::callScript<void>(app.scriptEngine, GLRT_ASSET_DIR"/common/scripts/hello-world.as", "void main()", "test");

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
