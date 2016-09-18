#ifndef GLRT_RENDERER_GUI_ANTTWEAKBAR_H
#define GLRT_RENDERER_GUI_ANTTWEAKBAR_H


#include <glrt/gui/toolbar.h>
#include <glrt/toolkit/uuid.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/renderer/debugging/surface-shader-visualizations.h>
#include <glrt/renderer/bvh-usage.h>

#include <glrt/scene/camera-parameter.h>


namespace glrt {

class Profiler;
class Application;

namespace scene {
class Scene;
} // namspace scene

namespace renderer {

class Renderer;

namespace debugging {

class ShaderDebugPrinter;

} // namspace debugging
} // namespace renderer

namespace gui {


using glrt::scene::CameraParameter;
using glrt::scene::Scene;


template<typename T, typename T_Map=QMap<QString, T>>
class TweakBarEnum
{
public:
  typedef T_Map Map;
  typedef QSharedPointer<TweakBarEnum<T, Map>> Ptr;

  static_assert(std::is_same<typename T_Map::key_type, QString>::value, "Expecting keytype QString");

  Map map;
  const std::string name;
  const std::string typeName;
  const TwType typeId;
  int currentIndex = 0;

  TweakBarEnum(const std::string& typeName, TwBar* tweakBar, const std::string& name, const char* def)
    : name(name),
      typeName(typeName),
      typeId(TwDefineEnumFromString(typeName.c_str(), "[None]")),
      tweakBarName(TwGetBarName(tweakBar))
  {
    TwAddVarCB(tweakBar, name.c_str(), typeId, reinterpret_cast<TwSetVarCallback>(setValue), reinterpret_cast<TwGetVarCallback>(getValue), this, def);
  }

  ~TweakBarEnum()
  {
    TwBar* tweakBar = TwGetBarByName(tweakBarName.c_str());

    if(tweakBar)
      TwRemoveVar(tweakBar, name.c_str());
  }

  template<typename T_uuid_inner_type>
  void initWithUuids(const glrt::scene::resources::ResourceManager& resourceManager, const QList<Uuid<T_uuid_inner_type>>& values)
  {
    Map map;

    for(const Uuid<T_uuid_inner_type>& value : values)
      map[resourceManager.labelForResourceUuid(value)] = value;

    init(map);
  }

  void init(const Map& map)
  {
    this->map = map;

    if(map.size() == 0)
    {
      currentIndex = 0;
      TwDefineEnumFromString(typeName.c_str(), "[None]");
      return;
    }

    currentIndex = glm::clamp<int>(currentIndex, 0, map.size()-1);

    std::vector<TwEnumVal> enumValues;
    QVector<char*> labels;
    enumValues.reserve(map.size());
    labels.reserve(map.size());

    int index=0;
    for(auto i=map.begin(); i!=map.end(); ++i)
    {
      char* label = new char[i.key().length()+1];
      memcpy(label, i.key().toStdString().c_str(), i.key().length()+1);
      labels.append(label);

      TwEnumVal v;
      v.Label = label;
      v.Value = index;

      enumValues.push_back(v);

      index++;
    }

    TwDefineEnum(typeName.c_str(), enumValues.data(), enumValues.size());

    for(char* l : labels)
      delete[] l;

    sendChangedSignal();
  }

  std::function<void(const T&)> valueChangedByUser;
  std::function<void(const T&)> valueChanged;

  void sendChangedSignal(bool byUser=false)
  {
    if(byUser)
    {
      if(valueChangedByUser && currentIndex>=0 && currentIndex<map.size())
        valueChangedByUser(map.values()[currentIndex]);
    }
    if(valueChanged && currentIndex>=0 && currentIndex<map.size())
      valueChanged(map.values()[currentIndex]);
  }

  void setIndex(int i)
  {
    if(i<0 || i==currentIndex)
      return;

    currentIndex = i;
    sendChangedSignal();
  }

  void setCurrentValue(const T& value)
  {
    int i = map.values().indexOf(value);
    setIndex(i);
  }

  void setCurrentKey(const QString& key)
  {
    int i = map.keys().indexOf(key);
    setIndex(i);
  }

private:
  std::string tweakBarName;

  static void getValue(int* value, TweakBarEnum<T, Map>* wrapper)
  {
    *value = wrapper->currentIndex;
  }

  static void setValue(const int* value, TweakBarEnum<T, Map>* wrapper)
  {
    if(wrapper->map.size() > *value && *value >= 0)
    {
      wrapper->currentIndex = *value;
      wrapper->sendChangedSignal(true);
    }
  }
};






template<typename T>
class TweakBarCBVar
{
public:
  std::function<T()> getter;
  std::function<void(T)> setter;

  void operator=(renderer::GLSLMacroWrapper<T>& macroWrapper)
  {
    getter = [&macroWrapper](){return macroWrapper.get_value();};
    setter = [&macroWrapper](T value){macroWrapper.set_value(value);};
  }

  void operator=(VariableWithCallback<T>& varWrapper)
  {
    getter = [&varWrapper](){return varWrapper.get_value();};
    setter = [&varWrapper](T value){varWrapper.set_value(value);};
  }

  void TwAddVarCB(TwBar* bar, const char* name, const char* def)
  {
    ::TwAddVarCB(bar, name, type(), reinterpret_cast<TwSetVarCallback>(setValue), reinterpret_cast<TwGetVarCallback>(getValue), this, def);
  }

  void reapply()
  {
    if(getter && setter)
      setter(getter());
  }

  static TwType type();

private:

  static void getValue(T* value, TweakBarCBVar<T>* wrapper)
  {
    if(wrapper->getter)
      *value = wrapper->getter();
  }

  static void setValue(const T* value, TweakBarCBVar<T>* wrapper)
  {
    if(wrapper->setter)
      wrapper->setter(*value);
  }
};

template<>
inline TwType TweakBarCBVar<bool>::type()
{
  return TW_TYPE_BOOLCPP;
}

template<>
inline TwType TweakBarCBVar<int>::type()
{
  return TW_TYPE_INT32;
}

template<>
inline TwType TweakBarCBVar<uint16_t>::type()
{
  return TW_TYPE_UINT16;
}

template<>
inline TwType TweakBarCBVar<float>::type()
{
  return TW_TYPE_FLOAT;
}


class TweakBarShaderToggle : public TweakBarCBVar<bool>
{
public:
  TweakBarShaderToggle(const QString& macroName);
};


class AntTweakBar final : public QObject
{
  Q_OBJECT
public:
  struct Settings final
  {
    QString globalDescription;
    bool showByDefault = false;
    bool toggleHelp = false;
    bool toggleGui= false;
    padding<bool, 5> _padding;

    static Settings sampleGui(const QString& globalDescription)
    {
      Settings settings;

      settings.showByDefault = true;
      settings.globalDescription = globalDescription;
      settings.toggleHelp = true;
      settings.toggleGui = true;

      return settings;
    }
  };

public:
  Application* application;
  gui::Toolbar toolbar;

  bool visible;
  bool toggleHelp;
  bool toggleGui;

  // The caller owns the given instance.
  // You must ensure, that the given application lives longer than the newly constructed instance.
  AntTweakBar(Application *application, const Settings& settings);
  ~AntTweakBar();

  AntTweakBar(const AntTweakBar&) = delete;
  AntTweakBar(const AntTweakBar&&) = delete;
  AntTweakBar& operator=(const AntTweakBar&) = delete;
  AntTweakBar& operator=(const AntTweakBar&&) = delete;

  TwBar* createCustomBar(QString name,
                         QString help,
                         glm::ivec2 pos = glm::ivec2(65535, 0),
                         glm::ivec2 size = glm::ivec2(256),
                         int marginToWindowBorder = 32);

  // The caller owns the given instance.
  // You must ensure, that the given profiler lives longer than the created bar
  TwBar* createProfilerBar(Profiler* profiler);

  // The caller owns the given instance.
  // You must ensure, that the given rederer lives longer than the created bar
  TwBar* createDebugSceneBar(renderer::Renderer* renderer);

  // The caller owns the given instance.
  // You must ensure, that the given debugPrinter lives longer than the created bar.
  // Note: debugPritnercan be null
  TwBar* createDebugShaderBar(renderer::Renderer* renderer, renderer::debugging::ShaderDebugPrinter* debugPrinter=nullptr);


  bool handleEvents(const SDL_Event& event);
  inline void update(float deltaTime){Q_UNUSED(deltaTime);}
  void draw();


private:
  typedef TweakBarEnum<Uuid<Scene>> SceneEnumeration;
  typedef TweakBarEnum<QPointer<scene::CameraComponent>> CameraEnumeration;
  typedef TweakBarEnum<glrt::renderer::SurfaceShaderVisualization> VisualizationEnumeration;
  typedef TweakBarEnum<glrt::renderer::BvhUsage> BvhUsageEnumeration;

  gui::TweakBarCBVar<bool> toggleProfiler;
  gui::TweakBarCBVar<bool> roughnessAdjustmentToggle;
  gui::TweakBarCBVar<bool> sdfShadowsToggle;
  gui::TweakBarCBVar<bool> updateGridCameraToggle;
  gui::TweakBarCBVar<bool> disableSceneryVoxels;
  gui::TweakBarCBVar<uint16_t> numBvhGrids;
  gui::TweakBarCBVar<uint16_t> bvhStackDepth;

  TweakBarCBVar<float> SDFSAMPLING_SPHERETRACING_START;
  TweakBarCBVar<float> SDFSAMPLING_SELF_SHADOW_AVOIDANCE;
  TweakBarCBVar<int> SDFSAMPLING_EXPONENTIAL_NUM;
  TweakBarCBVar<float> SDFSAMPLING_EXPONENTIAL_START;
  TweakBarCBVar<float> SDFSAMPLING_EXPONENTIAL_FACTOR;
  TweakBarCBVar<float> SDFSAMPLING_EXPONENTIAL_OFFSET;

  bool _disableSceneryVoxels = false;

  enum class MouseCaptureState : int
  {
    IDLE,
    CAPTURE_NOW,
    CAPTURE_NEXT_FRAME
  };

  MouseCaptureState captureMouseDeferred = MouseCaptureState::IDLE;
  void deferredMouseCapture();

  SceneEnumeration::Ptr sceneSwitcher;
  CameraEnumeration::Ptr cameraSwitcher;
  VisualizationEnumeration::Ptr visualizationSwitcher;
  BvhUsageEnumeration::Ptr bvhUsageSwitcher;

  TweakBarShaderToggle toggleLogHeatVision_debugPosteffect;
  TweakBarShaderToggle toggleLogHeatVision_costs;
  TweakBarShaderToggle toggleLogHeatVisionColors;

  TweakBarShaderToggle toggleDistancefieldFixedSamplePoints;
  TweakBarShaderToggle toggleDistancefieldAOSphereTracing;

  TweakBarShaderToggle toggleConeBouquetNoise;
  TweakBarShaderToggle toggleConeBouquetUnderwaterCaustics;

  void updateAntTweakBarWindowSize();

  void handeledEvent(const SDL_Event& event);
  bool unhandeledEvent(const SDL_Event& event);

  static void switchDebugCameraTo(Scene* scene, const QPointer<scene::CameraComponent>& otherCamera);

private slots:
  void handleSceneLoaded(Scene* scene);

  void setTweaBarAllocation(TwBar* tweakBar, glm::ivec2 pos, glm::ivec2 size, const glm::ivec2& marginToWindowBorder);
};



} // namespace gui
} // namespace glrt

#endif // GLRT_RENDERER_GUI_ANTTWEAKBAR_H
