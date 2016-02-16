#include <glrt/renderer/gl/command-list-recorder.h>

namespace gl {

CommandListRecorder::CommandListRecorder()
{

}

CommandListRecorder::~CommandListRecorder()
{
}

CommandListRecorder::CommandListRecorder(CommandListRecorder&& other)
  : commandTokens(std::move(other.commandTokens)),
    offsets(std::move(other.offsets)),
    sizes(std::move(other.sizes)),
    states(std::move(other.states)),
    fbos(std::move(other.fbos))
{
}

CommandListRecorder& CommandListRecorder::operator=(CommandListRecorder&& other)
{
  this->commandTokens.swap(other.commandTokens);
  this->offsets.swap(other.offsets);
  this->sizes.swap(other.sizes);
  this->states.swap(other.states);
  this->fbos.swap(other.fbos);
  return *this;
}


void CommandListRecorder::beginTokenList()
{
  Q_ASSERT(!isInsideBeginEnd());
  // #TODO
}

glm::ivec2 CommandListRecorder::endTokenList()
{
  Q_ASSERT(isInsideBeginEnd());
  // #TODO
}

void CommandListRecorder::append_drawcall(const glm::ivec2& tokens, const gl::StatusCapture* statusCapture, const gl::FramebufferObject* fbo)
{
  Q_ASSERT(!isInsideBeginEnd());

  offsets.append(tokens[0]);
  sizes.append(tokens[1]-tokens[0]);
  states.append(statusCapture);
  fbos.append(fbo);
}

CommandList&& CommandListRecorder::compile(glrt::Array<CommandListRecorder>&& segments)
{

  GLuint num_segments = static_cast<GLuint>(segments.length());

  CommandList commandList = CommandList::create(num_segments);


  for(int i=0; i<segments.length(); ++i)
  {
    CommandListRecorder segment = std::move(segments[i]);
    Q_ASSERT(!segment.isInsideBeginEnd());
    commandList.setSegment(GLuint(i), segment.createIndirectsArray(), segment.sizes, segment.states, segment.fbos);
  }

  glCompileCommandListNV(commandList.glhandle());

  return std::move(commandList);
}

glrt::Array<const void*> CommandListRecorder::createIndirectsArray() const
{
  Q_ASSERT(!isInsideBeginEnd());

  glrt::Array<const void*> indirects;

  indirects.reserve(offsets.length());

  for(int i=0; i<indirects.length(); ++i)
    indirects.append(commandTokens.data() + offsets[i]);

  return std::move(indirects);
}


} // namespace gl
