#include <glrt/scene/input-handler.h>

namespace glrt {
namespace scene {


InputHandler::InputHandler()
{
}

InputHandler::~InputHandler()
{
}


// -----------------------------------------------------------------------------


InputHandler::Priority::Priority()
  : Priority(0, 0)
{
}


InputHandler::Priority::Priority(type category, type priority)
  : category(category),
    priority(priority)
{
}


bool InputHandler::Priority::operator<(const Priority& other) const
{
  if(this->category < other.category)
    return true;
  if(this->category > other.category)
    return false;

  return this->priority < other.priority;
}


// =============================================================================


InputHandler::Manager::Manager()
{
}

InputHandler::Manager::~Manager()
{
}

void InputHandler::Manager::update()
{
  for(auto i = inputHandlers.begin(); i!=inputHandlers.end();)
  {
    if(i.value().isNull())
      i = inputHandlers.erase(i);
    else
      ++i;
  }
}

bool InputHandler::Manager::handleEvent(const SDL_Event& event)
{
  QList<QPointer<InputHandler>> listOfHandlers = inputHandlers.values();

  for(const QPointer<InputHandler>& handler : listOfHandlers)
  {
    if(handler->handleEvent(event))
      return true;
  }

  return false;
}

void InputHandler::Manager::addHandler(InputHandler* handler)
{
  inputHandlers.insert(handler->priority, handler);
}


} // namespace scene
} // namespace glrt
