#include <glrt/sample-application.h>
#include <glrt/renderer/bvh-usage.h>
#include <glrt/renderer/scene-renderer.h>
#include <glrt/renderer/debugging/surface-shader-visualizations.h>

#include <QSvgGenerator>
#include <QPainter>

using namespace glrt::renderer;
using namespace glrt::scene;

namespace glrt {
namespace gui {
  extern int default_camera_index;
} // namespace gui
} // namespace glrt

using glrt::gui::default_camera_index;

template<typename T>
T pickNearest(T value, const QVector<T>& options)
{
  T bestCandidate = value;

  for(T candidate : options)
    if(glm::abs(value-candidate) < glm::abs(value-bestCandidate))
      bestCandidate = candidate;

  return bestCandidate;
}


int main(int argc, char** argv)
{
  glm::uvec2 resolution(1920, 1080);

  float max_time = 5.f;
  uint32_t max_num_frames = 500;
  uint32_t frame_num_padding = 0;
  bool deferred = false;
  Q_UNUSED(deferred);

  SurfaceShaderVisualization surfaceShaderVisualization = SurfaceShaderVisualization::NONE;
  BvhUsage bvhUsage = BvhUsage::NO_BVH;
  QString screenshot_file_path;
  QString scala_file_path;
  QString framedurations_file_path;
  QString systeminfo_file_path;

  QVector<float> all_frame_times;

  QMap<QString, float> float_values;
  QMap<QString, std::function<void(float)>> float_setters;
  QMap<QString, bool> bool_values;
  QMap<QString, std::function<void(bool)>> bool_setters;
  QMap<QString, uint32_t> uint32_values;
  QMap<QString, std::function<void(uint32_t)>> uint32_setters;
  QMap<QString, uint16_t> uint16_values;
  QMap<QString, std::function<void(uint16_t)>> uint16_setters;

  QMap<QString, std::function<void(uint32_t)>> uint32_immediate_setters;

  Renderer* renderer;

  float_setters["--ao_bouquet_noise_rotation_angle"] = [](float v){AO_BOUQUET_NOISE_ROTATION_ANGLE.set_value(v);};
  float_setters["--spheretracing_min_step_size"] = [](float v){AO_SPHERETRACE_MINSTEPSIZE.set_value(v);};
  uint32_setters["--spheretracing_max_num_samples"] = [](uint32_t v){SDFSAMPLING_SPHERE_TRACING_MAX_NUM_LOOPS.set_value(int(v));};
  float_setters["--spheretracing_first_sample"] = [](float v){SDFSAMPLING_SPHERETRACING_START.set_value(v);};
  float_setters["--spheretracing_self_shadowavoidance"] = [](float v){SDFSAMPLING_SELF_SHADOW_AVOIDANCE.set_value(v);};
  bool_setters["--use_ao_spheretracing"] = [&](bool v){glrt::renderer::ReloadableShader::defineMacro("DISTANCEFIELD_AO_SPHERE_TRACING", v);};
  bool_setters["--use_fixed_ao_samples"] = [&](bool v){glrt::renderer::ReloadableShader::defineMacro("DISTANCEFIELD_FIXED_SAMPLE_POINTS", v);};
  bool_setters["--ao_spheretrace_clamping_correction"] = [&](bool v){AO_SPHERETRACE_CLAMPING_CORRECTION.set_value(v);};
  bool_setters["--heatvision_use_logarithmic_scale"] = [&](bool v){glrt::renderer::ReloadableShader::defineMacro("LOG_HEATVISION_DEBUG_COSTS", v);};
  bool_setters["--heatvision_use_colors"] = [&](bool v){glrt::renderer::ReloadableShader::defineMacro("HEATVISION_COLORS", v);};
  uint32_immediate_setters["--sdf_enforce_huge_bvh_leaves_first"] = [](bool v){ENFORCE_HUGE_BVH_LEAVES_FIRST.set_value(v);};
  uint32_immediate_setters["--sdf_sort_objects_by_sdf_texture"] = [](bool v){SORT_OBJECTS_BY_SDF_TEXTURE.set_value(v);};
  uint16_setters["--ao_falloff_none"] = [](bool v){AO_FALLBACK_NONE.set_value(v);};
  uint16_setters["--ao_falloff_clamped"] = [](bool v){AO_FALLBACK_CLAMPED.set_value(v);};
  uint16_setters["--ao_falloff_linear"] = [](bool v){AO_FALLBACK_LINEAR.set_value(v);};
  uint32_setters["--heatvision_black_level"] = [&](uint32_t v){renderer->costsHeatvisionBlackLevel = v;};
  uint32_setters["--heatvision_white_level"] = [&](uint32_t v){renderer->costsHeatvisionWhiteLevel = v;};
  uint32_setters["--frame_num_padding"] = [&](uint32_t v){frame_num_padding = v;};
  uint32_setters["--max_num_frames"] = [&](uint32_t v){max_num_frames = v;};
  uint16_setters["--num-bvh-grids"] = [](uint16_t v){NUM_GRID_CASCADES.set_value(glm::clamp<uint16_t>(v, 1, 3));};
  uint16_setters["--bvh-stack-depth"] = [](uint16_t v){BVH_MAX_STACK_DEPTH.set_value(glm::clamp<uint16_t>(v, 1, MAX_NUM_STATIC_MESHES));};
  uint16_setters["--num_cones"] = [](uint16_t v){N_GI_CONES.set_value(v<8 ? 7 : 9);};
  bool_setters["--spheretracing_bounding_sphere_clamping"] = [](bool v){SPHERETRACING_BOUNDING_SPHERE_CLAMPING.set_value(v);};
  // TODO: BEfehl um Groundtruth zu erstellen (einmalmit maximlaer Qualität ohne noise, dann mit Noise, mit Durchschnitt über viele Bilder hinweg)

  static_assert(MAX_SDF_CANDIDATE_GRID_SIZE==32, "Please adapt the line below to cover all possible candidate grid sizes");
  uint16_setters["--sdf_scandidate_grid_size"] = [](uint16_t v){SDF_CANDIDATE_GRID_SIZE.set_value(pickNearest(v, {16, 32}));};
  bool_setters["--ao_use_candidate_grid"] = [](bool v){AO_USE_CANDIDATE_GRID.set_value(v);};
  bool_setters["--ao_ignore_fallback_sdf"] = [](bool v){AO_IGNORE_FALLBACK_SDF.set_value(v);};
  bool_setters["--ao_fallback_sdf_only"] = [](bool v){AO_FALLBACK_SDF_ONLY.set_value(v);};
  bool_setters["--ao_enable_boundingsphere_culling"] = [](bool v){AO_ENABLE_BOUNDINGSPHERE_CULLING.set_value(v);};
  bool_setters["--noisy"] = [](bool v){CONE_BOUQUET_NOISE.set_value(v);};
  bool_setters["--low_res"] = [&resolution](bool v){resolution = v ? glm::uvec2(1024,768) : glm::uvec2(1920,1080);};
  uint32_setters["--merged_static_sdf_size"] = [](uint32_t v){MERGED_STATIC_SDF_SIZE.set_value(v);};
  uint32_immediate_setters["--default_camera_index"] = [](uint32_t v){default_camera_index=int(glm::clamp<uint32_t>(v, 0, 1024));};

  float_setters["--fallback_fading_start"] = [](float v){AO_STATIC_FALLBACK_FADING_START.set_value(v);};
  float_setters["--fallback_fading_end"] = [](float v){AO_STATIC_FALLBACK_FADING_END.set_value(v);};

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
        qDebug() << "--surface-visualization " << arguments.first();

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
    }else if(arguments.first() == "--scala")
    {
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        scala_file_path = arguments.first();
        arguments.removeFirst();

        QFileInfo scala_file(scala_file_path);
        if(!scala_file.dir().mkpath("."))
          qWarning() << "Couldn't create directory"<<scala_file.absolutePath();
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
    }else if(uint32_setters.contains(arguments.first()))
    {
      QString name = arguments.first();
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        uint32_values[name] = arguments.first().toUInt(&ok);
        arguments.removeFirst();
      }
    }else if(uint32_immediate_setters.contains(arguments.first()))
    {
      QString name = arguments.first();
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        uint32_t v = arguments.first().toUInt(&ok);
        arguments.removeFirst();
        uint32_immediate_setters[name](v);
      }
    }else if(uint16_setters.contains(arguments.first()))
    {
      QString name = arguments.first();
      arguments.removeFirst();

      if(arguments.length() >= 1)
      {
        uint16_values[name] = static_cast<uint16_t>(glm::clamp<uint32_t>(arguments.first().toUInt(&ok), 0, std::numeric_limits<uint16_t>::max()));
        arguments.removeFirst();
      }
    }

    if(!ok)
    {
      qWarning() << "invalid argument" << argument;
      return 1;
    }
  }

  max_num_frames += frame_num_padding*2;
  all_frame_times.reserve(int(max_num_frames+frame_num_padding*3));

  glrt::SampleApplication app(argc, argv,
                              glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to use the forward renderer to render a simple scene" // help text of the sample
                                                                          ),
                              deferred ? glrt::SampleApplication::Settings::techDemoDeferred() : glrt::SampleApplication::Settings::techDemoForward(),
                              glrt::Application::Settings::techDemo(),
                              glrt::System::Settings::addVSync(glrt::System::Settings::fullscreen("Benchmark", resolution), false));
  renderer = app.renderer;


  SDL_ShowCursor(0);

  app.showWindow();

  app.drawSingleFrame();

  {
    ReloadableShader::DeferredCompilation deferredCompilation;

    setCurrentBVHUsage(bvhUsage);
    setCurrentSurfaceShaderVisualization(surfaceShaderVisualization);

    for(const QString& name : float_values.keys())
      float_setters[name](float_values[name]);
    for(const QString& name : bool_values.keys())
      bool_setters[name](bool_values[name]);
    for(const QString& name : uint32_values.keys())
      uint32_setters[name](uint32_values[name]);
    for(const QString& name : uint16_values.keys())
      uint16_setters[name](uint16_values[name]);

    Q_UNUSED(deferredCompilation);
  }

  app.antweakbar.visible = false;

  app.drawSingleFrame();

  uint32_t num_frames = 0;
  float total_time = 0;
  bool aborted_by_criteria = false;
  while(app.isRunning)
  {
    const float deltaTime = app.drawSingleFrame();

    all_frame_times << deltaTime;
    aborted_by_criteria = (num_frames++) >= max_num_frames || (total_time+=deltaTime) > max_time;
    bool need_more_frames = num_frames<frame_num_padding*3+1;
    app.isRunning = app.isRunning && (!aborted_by_criteria || need_more_frames);
  }

  if(!screenshot_file_path.isEmpty())
  {
    // save screenshot to file
    QImage screenshot = app.renderer->takeScreenshot();
    screenshot.save(screenshot_file_path);

//    QString hdrPath = QFileInfo(screenshot_file_path).dir().absoluteFilePath(QFileInfo(screenshot_file_path).baseName() + ".exr");
  }

  if(!scala_file_path.isEmpty())
  {
    bool logarithmic_scale = glrt::renderer::ReloadableShader::isMacroDefined("LOG_HEATVISION_DEBUG_COSTS");

    QSvgGenerator svg;
    svg.setResolution(300);
    QPoint pos(0, 0);
    QSize size(svg.resolution()*2, svg.resolution()*5);
    qreal pt = svg.resolution() / 72.; // https://de.wikipedia.org/wiki/Schriftgrad#DTP-Punkt
    svg.setFileName(scala_file_path);
    svg.setTitle(QString("%0_%1_%2")
                 .arg(QFileInfo(scala_file_path).baseName())
                 .arg(renderer->costsHeatvisionWhiteLevel)
                 .arg(logarithmic_scale ? "_log" : ""));
    svg.setSize(size);
    svg.setViewBox(QRect(pos, size));

    QPainter painter;
    painter.begin(&svg);

    qreal gradient_width = 40*pt;

    QPen pen(QColor::fromRgb(0));
    pen.setWidthF(pt);
    pen.setJoinStyle(Qt::SvgMiterJoin);

    qreal padding = 10*pt + 0.5*pt;

    qreal start = size.height()-padding;
    qreal end = pt*0.5 + padding;

    QLinearGradient gradient(0., start, 0., end);
    qreal f = 1./7.;
    gradient.setColorAt(0*f, QColor::fromRgb(0));
    gradient.setColorAt(1*f, QColor::fromRgb(0x204a87));
    gradient.setColorAt(2*f, QColor::fromRgb(0x5c3566));
    gradient.setColorAt(3*f, QColor::fromRgb(0x75507b));
    gradient.setColorAt(4*f, QColor::fromRgb(0xcc0000));
    gradient.setColorAt(5*f, QColor::fromRgb(0xf57900));
    gradient.setColorAt(6*f, QColor::fromRgb(0xfce94f));
    gradient.setColorAt(7*f, QColor::fromRgb(0xffffff));

    painter.setPen(pen);
    for(int i=0; i<=7; ++i)
    {
      qreal normalized_value = i*f;

      const uint32_t whiteLevel = renderer->costsHeatvisionWhiteLevel;
      const uint32_t blackLevel = renderer->costsHeatvisionBlackLevel;
      const uint32_t maxValue = whiteLevel - blackLevel;
      qreal linear_value = glm::mix<qreal>(blackLevel, whiteLevel, normalized_value);
      qreal log_value = glm::exp2(normalized_value * glm::log2(qreal(maxValue))) + blackLevel;

      qreal value;
      if(logarithmic_scale)
        value = log_value;
      else
        value = linear_value;

      qreal pos = glm::mix(start, end, normalized_value);

      painter.drawLine(QLineF(gradient_width + 4*pt, pos, gradient_width + 16*pt, pos));
      painter.drawText(QRectF(gradient_width + 20*pt, pos-0*pt, 50*pt, 0*pt), Qt::AlignVCenter, QString("%0").arg(glm::round(value)));
    }

    // IDEA: histogram

    painter.setBrush(QBrush(gradient));
    painter.drawRect(QRectF(pt*0.5, padding, gradient_width-pt, size.height()-padding*2));

    painter.end();
  }

  if(!framedurations_file_path.isEmpty())
  {
    QFile file(framedurations_file_path);
    if(!file.open(QFile::WriteOnly))
    {
      qWarning() << "Couldn't open the file for writing the framerate data";
      return 1;
    }
    for(uint32_t i=frame_num_padding; i<uint32_t(all_frame_times.length())-frame_num_padding; ++i)
    {
      if(i!=frame_num_padding)
        file.write("\n");
      file.write(QString::number(qreal(all_frame_times[int(i)]), 'g', 10).toUtf8());
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
