#ifndef GLRT_GUI_ANTTWEAKBAR_H
#define GLRT_GUI_ANTTWEAKBAR_H


#include <glrt/gui/toolbar.h>

#include <glrt/scene/camera-parameter.h>


namespace glrt {


class Application;
class Profiler;

namespace scene {

class Renderer;
class Scene;

} // namspace scene


namespace gui {



template<typename T>
class TweakBarEnum
{
public:
  typedef QSharedPointer<TweakBarEnum<T>> Ptr;
  typedef QMap<QString, T> Map;

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
  }

  std::function<void(const T&)> valueChanged;

  void setCurrentValue(const T& value)
  {
    int i = map.values().indexOf(value);
    if(i>=0)
      currentIndex = i;
  }

  void setCurrentKey(const QString& key)
  {
    int i = map.keys().indexOf(key);
    if(i>=0)
      currentIndex = i;
  }

private:
  std::string tweakBarName;

  static void getValue(int* value, TweakBarEnum<T>* wrapper)
  {
    *value = wrapper->currentIndex;
  }

  static void setValue(const int* value, TweakBarEnum<T>* wrapper)
  {
    if(wrapper->map.size() > *value && *value >= 0 && wrapper->valueChanged)
      wrapper->valueChanged(wrapper->map.values()[*value]);
  }
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
  TwBar* createDebugSceneBar(scene::Renderer* renderer);


  bool handleEvents(const SDL_Event& event);
  inline void update(float deltaTime){Q_UNUSED(deltaTime);}
  void draw();


private:
  TweakBarEnum<QString>::Ptr sceneSwitcher;
  TweakBarEnum<scene::CameraParameter>::Ptr cameraSwitcher;

  void updateAntTweakBarWindowSize();

  void handeledEvent(const SDL_Event& event);
  bool unhandeledEvent(const SDL_Event& event);

private slots:
  void handleSceneLoaded(scene::Scene* scene);
};


template<typename T>
class TweakBarCBVar
{
public:
  std::function<T()> getter;
  std::function<void(T)> setter;

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


} // namespace gui
} // namespace glrt

#endif // GLRT_GUI_ANTTWEAKBAR_H
