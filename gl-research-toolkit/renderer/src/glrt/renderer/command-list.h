#ifndef GLRT_RENDERER_COMMANDLIST_H
#define GLRT_RENDERER_COMMANDLIST_H

#include <glrt/dependencies.h>
#include <nvcommandlist.h>

namespace glrt {
namespace renderer {

class CommandList
{
public:
  CommandList();
  ~CommandList();

  CommandList(CommandList&& other);
  CommandList& operator=(CommandList&& other);

  CommandList(const CommandList&) = delete;
  CommandList&operator=(const CommandList&) = delete;

  void call();

private:
  GLuint _commandListHandler;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMMANDLIST_H
