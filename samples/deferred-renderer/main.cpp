#include <glrt/sample-application.h>
#include <glrt/renderer/scene-renderer.h>

const bool ADD_VSYNC = true;

#define RECORD_FRAMES 0

int main(int argc, char** argv)
{
  glrt::renderer::show_forward_only = false;

  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the deferred renderer to render a simple scene" // help text of the sample
                                                                          ),
                              glrt::SampleApplication::Settings::techDemoDeferred(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(
#if RECORD_FRAMES
                                glrt::System::Settings::fullscreen("Deferred-Renderer"),
#else
                                glrt::System::Settings::simpleWindow("Deferred-Renderer"),
#endif
                                ADD_VSYNC));

  app.showWindow();

  // Workaround for driver issues. I will never use untested extension again :(
  app.drawSingleFrame();
  app.renderer->setAmbientOcclusionSDF(true);
  app.renderer->setSDFShadows(true);

#if RECORD_FRAMES
  QDir screenshot_dir = QDir::current().absoluteFilePath("recording");
  if(screenshot_dir.exists())
    screenshot_dir.removeRecursively();
  if(!screenshot_dir.mkpath("."))
    qWarning() << "Couldn't create dir <" << screenshot_dir.absolutePath() << "> for saving the recorded screenshots";
  QString screenshot_path = screenshot_dir.absoluteFilePath("screenshot-%0.bmp");
  uint frame_index = 0;
#endif

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

#if RECORD_FRAMES
    QImage screenshot = app.renderer->takeScreenshot();
    QString formated_frame_index = QString::number(frame_index++, 10);
    if(Q_UNLIKELY(frame_index < 10))
      formated_frame_index = "00" + formated_frame_index;
    else if(Q_UNLIKELY(frame_index < 100))
      formated_frame_index = "0" + formated_frame_index;
    screenshot.save(screenshot_path.arg(formated_frame_index));
#endif
  }

  return 0;
}
