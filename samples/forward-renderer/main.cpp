#include <glrt/sample-application.h>
#include <glrt/renderer/scene-renderer.h>

const bool ADD_VSYNC = true;

int main(int argc, char** argv)
{
  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                          ),
                              glrt::SampleApplication::Settings::techDemoForward(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(glrt::System::Settings::simpleWindow("Forward-Renderer"), ADD_VSYNC));

  app.showWindow();

  // Workaround for driver issues. I will never use untested extension again :(
  app.drawSingleFrame();
  app.renderer->setAmbientOcclusionSDF(true, false);
  app.renderer->setSDFShadows(true);

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
