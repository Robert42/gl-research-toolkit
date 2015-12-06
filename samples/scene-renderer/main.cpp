#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/forward-renderer.h>
#include <glrt/gui/anttweakbar.h>

#include <glhelper/gl.hpp>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Scene-Renderer"));

  glrt::scene::Scene scene(app.sdlWindow);
  glrt::scene::ForwardRenderer renderer(&scene);

  glrt::gui::AntTweakBar antweakbar(&app,
                                    glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to load and display a simple single scene" // help text of the sample
                                                                                ));

  antweakbar.createDebugSceneBar(&renderer);
  antweakbar.createProfilerBar(&app.profiler);

  scene.loadFromFile(GLRT_ASSET_DIR"/common/scenes/cornell-box/cornell-box.scene");

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
      if(scene.handleEvents(event))
        continue;
      if(antweakbar.handleEvents(event))
        continue;
    }

    const float deltaTime = app.update();
    scene.update(deltaTime);
    antweakbar.update(deltaTime);

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    renderer.render();

    antweakbar.draw();

    app.swapWindow();
  }

  return 0;
}
