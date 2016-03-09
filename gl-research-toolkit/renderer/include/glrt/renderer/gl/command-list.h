#ifndef GL_COMMANDLIST_H
#define GL_COMMANDLIST_H

#include <glrt/toolkit/array.h>

#include <glhelper/framebufferobject.hpp>
#include <glhelper/buffer.hpp>

#include <nvcommandlist.h>

namespace gl {

class StatusCapture;
class CommandList final
{
  Q_DISABLE_COPY(CommandList)
public:
  CommandList();
  ~CommandList();

  static CommandList create(GLuint num_segments);

  CommandList(CommandList&& other);
  CommandList& operator=(CommandList&& other);

  void setSegment(GLuint segment,
                  const glrt::Array<const void*>& indirects,
                  const glrt::Array<GLsizei>& sizes,
                  const glrt::Array<const StatusCapture*>& states,
                  const glrt::Array<const FramebufferObject*>& fbos);
  void call();

  GLuint glhandle() const{return _commandListHandler;}

private:
  GLuint _commandListHandler = 0;
  GLuint num_segments = 0;
};

} // namespace gl

#endif // GL_COMMANDLIST_H
