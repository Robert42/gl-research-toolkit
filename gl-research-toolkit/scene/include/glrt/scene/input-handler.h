#ifndef GLRT_SCENE_INPUTHANDLER_H
#define GLRT_SCENE_INPUTHANDLER_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {

class InputHandler : public QObject
{
  Q_OBJECT
public:
  class Manager;
  struct Priority
  {
    typedef quint16 type;

    type category;
    type priority;

    Priority();
    Priority(type category, type priority);

    bool operator<(const Priority& other) const;
  };

  InputHandler();
  ~InputHandler();

  Priority priority;

  virtual bool handleEvent(const SDL_Event& event) = 0;
};

class InputHandler::Manager : public QObject
{
  Q_OBJECT
public:
  Manager();
  ~Manager();

  bool handleEvent(const SDL_Event& event);

  void update();
  void addHandler(InputHandler* handler);

private:
  QMultiMap<Priority, QPointer<InputHandler>> inputHandlers;
};

const InputHandler::Priority MAX_PRIORITY(std::numeric_limits<InputHandler::Priority::type>::max(), std::numeric_limits<InputHandler::Priority::type>::max());
const InputHandler::Priority DEBUG_CAMERA(MAX_PRIORITY.category-1024, 0);

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_INPUTHANDLER_H
