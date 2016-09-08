#include <glrt/sample-application.h>

int main(int argc, char** argv)
{
  const glm::uvec2 resolution(1920, 1080);

  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                          ),
                              glrt::SampleApplication::Settings::techDemo(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(glrt::System::Settings::fullscreen("Benchmark", resolution), false));

  app.showWindow();

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
      if(app.handleEvents(event))
        continue;

      // T O D O : Add your event handling code here
    }

    const float deltaTime = app.update();
    Q_UNUSED(deltaTime);

    app.beginDrawing();
    app.drawScene();

    // T O D O: add here your rendering code

    app.endDrawing();
    app.swapWindow();
  }

  return 0;
}
