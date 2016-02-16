#ifndef GLRT_RENDERER_GL_COMMANDLIST_H
#define GLRT_RENDERER_GL_COMMANDLIST_H

#include <glrt/dependencies.h>
#include <nvcommandlist.h>

namespace glrt {
namespace renderer {
namespace gl {

class CommandList
{
  Q_DISABLE_COPY(CommandList)
public:
  CommandList();
  ~CommandList();

  static CommandList&& create();

  CommandList(CommandList&& other);
  CommandList& operator=(CommandList&& other);

  void call();

  GLuint glhandle() const{return _commandListHandler;}

private:
  GLuint _commandListHandler;
};

} // namespace gl
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_GL_COMMANDLIST_H
