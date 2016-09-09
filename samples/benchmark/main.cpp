#include <glrt/sample-application.h>
#include <glrt/renderer/bvh-usage.h>
#include <glrt/renderer/debugging/surface-shader-visualizations.h>

using namespace glrt::renderer;

int main(int argc, char** argv)
{
  const glm::uvec2 resolution(1920, 1080);

  int max_num_frames = 500;
  float max_time = 5.f;
  bool deferred = false;

  SurfaceShaderVisualization surfaceShaderVisualization = SurfaceShaderVisualization::NONE;
  BvhUsage bvhUsage = BvhUsage::NO_BVH;
  QString screenshot_file_path;
  QString framedurations_file_path;

  QVector<float> all_frame_times;

  QStringList arguments;
  for(int i=1; i<argc; ++i)
    arguments << QString::fromUtf8(argv[i]).trimmed();

  while(!arguments.isEmpty())
  {
    QString argument = arguments.first();

    bool ok = false;
    if(arguments.first() == "--surface-visualization") // --visualization <VALUE>
    {
      arguments.removeFirst();
      auto map = allSurfaceShaderVisualizations();

      if(arguments.length() >= 1 && map.contains(arguments.first()))
      {
        surfaceShaderVisualization = map.value(arguments.first());
        ok = true;
        arguments.removeFirst();
      }
    }else if(arguments.first() == "--bvh-usage") // --visualization <VALUE>
    {
      arguments.removeFirst();
      auto map = allcurrentBvhUsages();

      if(arguments.length() >= 1 && map.contains(arguments.first()))
      {
        bvhUsage = map.value(arguments.first());
        ok = true;
        arguments.removeFirst();
      }
    }else if(arguments.first() == "--max_time") // --max_time <FLOAT_VALUE>
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        max_time = arguments.first().toFloat(&ok);
        arguments.removeFirst();
      }
    }else if(arguments.first() == "--max_num_frames") // --max_num_frames <FLOAT_VALUE>
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        max_num_frames = arguments.first().toInt(&ok);
        arguments.removeFirst();
      }
    }else if(arguments.first() == "--deferred") // --deferred
    {
      arguments.removeFirst();
      ok = true;
      deferred = true;
    }else if(arguments.first() == "--forward") // --forward
    {
      arguments.removeFirst();
      ok = true;
      deferred = false;
    }else if(arguments.first() == "--screenshot")
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        screenshot_file_path = arguments.first();
        arguments.removeFirst();

        QFileInfo screenshot_file(screenshot_file_path);
        if(!screenshot_file.dir().mkpath("."))
          qWarning() << "Couldn't create directory"<<screenshot_file.absolutePath();
        else
          ok = true;
      }
    }else if(arguments.first() == "--framerate")
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        framedurations_file_path = arguments.first();
        arguments.removeFirst();

        QFileInfo framedurations_file(screenshot_file_path);
        if(!framedurations_file.dir().mkpath("."))
          qWarning() << "Couldn't create directory"<<framedurations_file.absolutePath();
        else
          ok = true;
      }
    }

    if(!ok)
    {
      qWarning() << "invalid argument" << argument;
      return 1;
    }
  }

  setCurrentSurfaceShaderVisualization(surfaceShaderVisualization);
  setCurrentBVHUsage(bvhUsage);

  const int frame_count_padding = 4;
  max_num_frames += frame_count_padding*2;
  all_frame_times.reserve(max_num_frames);

  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                          ),
                              glrt::SampleApplication::Settings::techDemo(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(glrt::System::Settings::fullscreen("Benchmark", resolution), false));

  app.antweakbar.visible = false;

  SDL_ShowCursor(0);

  app.showWindow();

  int num_frames = 0;
  float total_time = 0;
  bool aborted_by_criteria = false;
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

    all_frame_times << deltaTime;
    aborted_by_criteria = (num_frames++) >= max_num_frames || (total_time+=deltaTime) > max_time;
    app.isRunning = app.isRunning && !aborted_by_criteria;
  }

  if(!screenshot_file_path.isEmpty())
  {

    QImage screenshot(int(resolution.x), int(resolution.y), QImage::Format::Format_RGBA8888);
    GLsizei length = GLsizei(screenshot.byteCount());
    GL_CALL(glReadnPixels, 0, 0, int(resolution.x), int(resolution.y), GL_RGBA,  GL_UNSIGNED_BYTE, length, screenshot.bits());
    for(int y=0; y<int(resolution.y/2); ++y)
    {
      byte* a = screenshot.scanLine(y);
      byte* b = screenshot.scanLine(int(resolution.y)-1-y);

      const int w = screenshot.bytesPerLine();
      for(int x=0; x<w; ++x)
        std::swap(a[x], b[x]);
    }
    screenshot.save(screenshot_file_path);
  }

  if(!framedurations_file_path.isEmpty())
  {
    QFile file(framedurations_file_path);
    if(!file.open(QFile::WriteOnly))
    {
      qWarning() << "Couldn't open the file for writing the framerate data";
      return 1;
    }
    for(int i=frame_count_padding; i<all_frame_times.length()-frame_count_padding; ++i)
    {
      if(i!=frame_count_padding)
        file.write("\n");
      file.write(QString::number(all_frame_times[i], 'g', 10).toUtf8());
    }
  }

  return aborted_by_criteria ? 0 : 42;
}
