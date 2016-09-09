#include <glrt/sample-application.h>

int main(int argc, char** argv)
{
  const glm::uvec2 resolution(1920, 1080);

  int max_num_frames = 1000;
  float max_time = 30.f;
  bool deferred = false;

  QVector<float> all_frame_times;
  QMap<QString, QString> gui_commands;

  QStringList arguments;
  for(int i=1; i<argc; ++i)
    arguments << QString::fromUtf8(argv[i]).trimmed();

  while(!arguments.isEmpty())
  {
    QString argument = arguments.first();

    bool ok = false;
    if(arguments.first() == "--gui") // --gui <VARNAME> <VALUE>
    {
      arguments.removeFirst();

      if(arguments.length() >= 2)
      {
        ok = true;
        gui_commands[arguments[0]] = arguments[1];
        arguments.removeFirst();
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
    }

    if(!ok)
    {
      qWarning() << "invalid argument" << argument;
      return 1;
    }
  }

  all_frame_times.reserve(max_num_frames);

  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                          ),
                              glrt::SampleApplication::Settings::techDemo(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(glrt::System::Settings::fullscreen("Benchmark", resolution), false));

  // TODO: app.antweakbar.showAllPanels(); // to make sure all commands succeed
  for(const QString& key : gui_commands.keys())
  {
    // TODO: app.antweakbar.setValue(key, gui_commands.value(key));
  }

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

  return aborted_by_criteria ? 0 : 42;
}
