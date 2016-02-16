#ifndef GL_COMMANDLISTRECORDER_H
#define GL_COMMANDLISTRECORDER_H

#include "command-list.h"
#include "status-capture.h"


namespace gl {
class CommandListRecorder;
} // namespace gl

namespace glrt {

template<>
struct DefaultTraits<gl::CommandListRecorder>
{
  typedef ArrayTraits_mCmOD<gl::CommandListRecorder> type;
};

} // namespace glrt

namespace gl {

class CommandListRecorder final
{
public:
  Q_DISABLE_COPY(CommandListRecorder)

  CommandListRecorder();
  ~CommandListRecorder();

  CommandListRecorder(CommandListRecorder&& other);
  CommandListRecorder& operator=(CommandListRecorder&& other);

  void beginTokenList();
  // #TODO : append_token_ ...()
  glm::ivec2 endTokenList();

  void append_drawcall(const glm::ivec2& tokens, const gl::StatusCapture* statusCapture, const gl::FramebufferObject* fbo);

  static CommandList&& compile(glrt::Array<CommandListRecorder>&& segments);

private:
  glrt::Array<byte> commandTokens;
  glrt::Array<GLsizei> offsets;
  glrt::Array<GLsizei> sizes;
  glrt::Array<const gl::StatusCapture*> states;
  glrt::Array<const gl::FramebufferObject*> fbos;

  glrt::Array<const void*> createIndirectsArray() const;
  bool isInsideBeginEnd() const;
};

} // namespace gl

#endif // GL_COMMANDLISTRECORDER_H
