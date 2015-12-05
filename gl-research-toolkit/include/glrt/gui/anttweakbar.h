#ifndef GLRT_GUI_ANTTWEAKBAR_H
#define GLRT_GUI_ANTTWEAKBAR_H


#include <glrt/gui/toolbar.h>


namespace glrt {


class Application;
class Profiler;

namespace scene {

class Scene;

} // namspace scene


namespace gui {


class AntTweakBar final
{
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
  // You must ensure, that the given profiler lives longer than the created bar
  TwBar* createSceneBar(scene::Scene* scene);


  bool handleEvents(const SDL_Event& event);
  inline void update(float deltaTime){Q_UNUSED(deltaTime);}
  void draw();


private:
  void updateAntTweakBarWindowSize();

  void handeledEvent(const SDL_Event& event);
  bool unhandeledEvent(const SDL_Event& event);
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
