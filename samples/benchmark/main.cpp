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
  Q_UNUSED(deferred);

  SurfaceShaderVisualization surfaceShaderVisualization = SurfaceShaderVisualization::NONE;
  BvhUsage bvhUsage = BvhUsage::NO_BVH;
  QString screenshot_file_path;
  QString framedurations_file_path;
  QString systeminfo_file_path;

  QVector<float> all_frame_times;

  QMap<QString, float> float_values;
  QMap<QString, std::function<void(float)>> float_setters;
  QMap<QString, float> bool_values;
  QMap<QString, std::function<void(bool)>> bool_setters;

  float_setters["--spheretracing_first_sample"] = [](float v){SDFSAMPLING_SPHERETRACING_START.set_value(v);};
  float_setters["--spheretracing_self_shadowavoidance"] = [](float v){SDFSAMPLING_SELF_SHADOW_AVOIDANCE.set_value(v);};
  bool_setters["--use_ao_spheretracing"] = [&](bool v){glrt::renderer::ReloadableShader::defineMacro("DISTANCEFIELD_AO_SPHERE_TRACING", v);};
  bool_setters["--use_fixed_ao_samples"] = [&](bool v){glrt::renderer::ReloadableShader::defineMacro("DISTANCEFIELD_FIXED_SAMPLE_POINTS", v);};

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
    }else if(arguments.first() == "--num-bvh-grids") // --num-bvh-grids <VALUE in [1,3]>
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        set_num_grid_cascades(uint16_t(glm::clamp<uint32_t>(arguments.first().toUInt(&ok), 1, 3)));
        arguments.removeFirst();
      }
    }else if(arguments.first() == "--bvh-stack-depth") // --bvh-stack-depth <VALUE in [1,255]>
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        set_bvh_traversal_stack_depth(uint16_t(glm::clamp<uint32_t>(arguments.first().toUInt(&ok), 1, 255)));
        arguments.removeFirst();
      }
    }else if(arguments.first() == "--bvh-leaf-array-length") // --bvh-leaf-array-length <VALUE in [1,255]>
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        set_bvh_traversal_leaf_result_array_length(uint16_t(glm::min<uint32_t>(std::numeric_limits<uint16_t>::max(), arguments.first().toUInt(&ok))));
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
    }else if(arguments.first() == "--system-info")
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        systeminfo_file_path = arguments.first();
        arguments.removeFirst();

        QFileInfo systeminfo_file(screenshot_file_path);
        if(!systeminfo_file.dir().mkpath("."))
          qWarning() << "Couldn't create directory"<<systeminfo_file.absolutePath();
        else
          ok = true;
      }
    }else if(float_setters.contains(arguments.first()))
    {
      QString name = arguments.first();
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        float_values[name] = arguments.first().toFloat(&ok);
        arguments.removeFirst();
      }
    }else if(bool_setters.contains(arguments.first()))
    {
      QString name = arguments.first();
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        bool_values[name] = arguments.first().toInt(&ok) != 0;
        arguments.removeFirst();
      }
    }

    if(!ok)
    {
      qWarning() << "invalid argument" << argument;
      return 1;
    }
  }

  const int frame_count_padding = 4;
  max_num_frames += frame_count_padding*2;
  all_frame_times.reserve(max_num_frames+frame_count_padding*3);

  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                          ),
                              deferred ? glrt::SampleApplication::Settings::techDemoDeferred() : glrt::SampleApplication::Settings::techDemoForward(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(glrt::System::Settings::fullscreen("Benchmark", resolution), false));

  auto draw_single_frame = [&]() -> float {
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

     return deltaTime;
  };


  SDL_ShowCursor(0);

  app.showWindow();

  draw_single_frame();

  {
    // TODO defer recompiling

    setCurrentBVHUsage(bvhUsage);
    setCurrentSurfaceShaderVisualization(surfaceShaderVisualization);

    for(const QString& name : float_values.keys())
      float_setters[name](float_values[name]);
    for(const QString& name : bool_values.keys())
      bool_setters[name](bool_values[name]);
  }

  app.antweakbar.visible = false;

  draw_single_frame();

  int num_frames = 0;
  float total_time = 0;
  bool aborted_by_criteria = false;
  while(app.isRunning)
  {
    const float deltaTime = draw_single_frame();

    all_frame_times << deltaTime;
    aborted_by_criteria = (num_frames++) >= max_num_frames || (total_time+=deltaTime) > max_time;
    bool need_more_frames = num_frames<frame_count_padding*3+1;
    app.isRunning = app.isRunning && (!aborted_by_criteria || need_more_frames);
  }

  if(!screenshot_file_path.isEmpty())
  {
    // save screenshot to file
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

  if(!systeminfo_file_path.isEmpty())
  {
    QFile file(systeminfo_file_path);
    if(!file.open(QFile::WriteOnly))
    {
      qWarning() << "Couldn't open the file for writing the framerate data";
      return 1;
    }
    QTextStream textStream(&file);
    textStream << "gl_vendor=" << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << "\n";
    textStream << "gl_renderer=" << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << "\n";
    textStream << "gl_version=" << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << "\n";
    textStream << "shading_language_version=" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    textStream << "cache_line=" << SDL_GetCPUCacheLineSize() << "\n";
    textStream << "cpu_count=" << SDL_GetCPUCount() << "\n";
    textStream << "system_ram=" << SDL_GetSystemRAM() << "\n";
  }

  return aborted_by_criteria ? 0 : 42;
}
