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

  enum class FrontFace
  {
    CW = 0,
    CCW = 1,
  };
  enum class Strip : GLenum
  {
    STRIP,
    NO_STRIP
  };

  CommandListRecorder();
  ~CommandListRecorder();

  CommandListRecorder(CommandListRecorder&& other);
  CommandListRecorder& operator=(CommandListRecorder&& other);

  void beginTokenList();
  void append_token_TerminateSequence();
  void append_token_NOP();
  void append_token_DrawArrays(GLuint count, GLuint first, Strip strip = Strip::NO_STRIP);
  void append_token_AttributeAddress(GLuint index, GLuint64 gpuAddress);
  void append_token_FrontFace(FrontFace frontFace);
  glm::ivec2 endTokenList();

  void append_drawcall(const glm::ivec2& tokens, const gl::StatusCapture* statusCapture, const gl::FramebufferObject* fbo);

  static CommandList compile(glrt::Array<CommandListRecorder>&& segments);
  static CommandList compile(CommandListRecorder&& segment);

private:
  glrt::Array<byte> commandTokens;
  glrt::Array<GLsizei> offsets;
  glrt::Array<GLsizei> sizes;
  glrt::Array<const gl::StatusCapture*> states;
  glrt::Array<const gl::FramebufferObject*> fbos;
  GLsizei tokenBeginStart = -1;

  glrt::Array<const void*> createIndirectsArray() const;
  bool isInsideBeginEnd() const;

  template<typename T>
  void append_token(T* token, GLenum tokenId);
};

} // namespace gl

#endif // GL_COMMANDLISTRECORDER_H
