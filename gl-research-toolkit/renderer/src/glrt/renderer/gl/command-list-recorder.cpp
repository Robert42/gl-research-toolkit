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
    fbos(std::move(other.fbos)),
    tokenBeginStart(other.tokenBeginStart)
{
  other.tokenBeginStart = -1;
}

CommandListRecorder& CommandListRecorder::operator=(CommandListRecorder&& other)
{
  this->commandTokens.swap(other.commandTokens);
  this->offsets.swap(other.offsets);
  this->sizes.swap(other.sizes);
  this->states.swap(other.states);
  this->fbos.swap(other.fbos);
  std::swap(this->tokenBeginStart, other.tokenBeginStart);
  return *this;
}


void CommandListRecorder::beginTokenList()
{
  Q_ASSERT(!isInsideBeginEnd());

  tokenBeginStart = commandTokens.length();
}

template<typename T>
void CommandListRecorder::append_token(T* token, GLenum tokenId)
{
  Q_ASSERT(isInsideBeginEnd());

  token->header = glGetCommandHeaderNV(tokenId, sizeof(T));

  commandTokens.append_by_memcpy(token, sizeof(T));
}

inline GLuint addressHi(GLuint64 fullAddress)
{
  return fullAddress>>32;
}

inline GLuint addressLo(GLuint64 fullAddress)
{
  return fullAddress & 0xffffffff;
}

void CommandListRecorder::append_token_TerminateSequence()
{
  TerminateSequenceCommandNV token;
  append_token(&token, GL_TERMINATE_SEQUENCE_COMMAND_NV);
}

void CommandListRecorder::append_token_NOP()
{
  NOPCommandNV token;
  append_token(&token, GL_NOP_COMMAND_NV);
}

void CommandListRecorder::append_token_AttributeAddress(GLuint index, GLuint64 gpuAddress)
{
  AttributeAddressCommandNV token;
  token.index = index;
  token.addressLo = addressLo(gpuAddress);
  token.addressHi = addressHi(gpuAddress);
  append_token(&token, GL_ATTRIBUTE_ADDRESS_COMMAND_NV);
}

glm::ivec2 CommandListRecorder::endTokenList()
{
  Q_ASSERT(isInsideBeginEnd());

  glm::ivec2 range = glm::ivec2(tokenBeginStart, commandTokens.length());

  tokenBeginStart = -1;

  return range;
}

void CommandListRecorder::append_drawcall(const glm::ivec2& tokens, const gl::StatusCapture* statusCapture, const gl::FramebufferObject* fbo)
{
  Q_ASSERT(!isInsideBeginEnd());

  offsets.append(tokens[0]);
  sizes.append(tokens[1]-tokens[0]);
  states.append(statusCapture);
  fbos.append(fbo);
}

CommandList CommandListRecorder::compile(glrt::Array<CommandListRecorder>&& segments)
{
  SPLASHSCREEN_MESSAGE("Compiling command_list");

  GLuint num_segments = static_cast<GLuint>(segments.length());

  CommandList commandList = CommandList::create(num_segments);


  for(int i=0; i<segments.length(); ++i)
  {
    CommandListRecorder segment = std::move(segments[i]);
    Q_ASSERT(!segment.isInsideBeginEnd());
    commandList.setSegment(GLuint(i), segment.createIndirectsArray(), segment.sizes, segment.states, segment.fbos);
  }

  GL_CALL(glCompileCommandListNV, commandList.glhandle());

  return std::move(commandList);
}

CommandList CommandListRecorder::compile(CommandListRecorder&& segment)
{
  glrt::Array<CommandListRecorder> segments;
  segments.append(std::move(segment));
  return compile(std::move(segments));
}

glrt::Array<const void*> CommandListRecorder::createIndirectsArray() const
{
  Q_ASSERT(!isInsideBeginEnd());

  glrt::Array<const void*> indirects;

  indirects.reserve(offsets.length());

  for(int i=0; i<offsets.length(); ++i)
  {
    Q_ASSERT(commandTokens.length() >= offsets[i]+sizes[i]);
    indirects.append(commandTokens.data() + offsets[i]);
  }

  return std::move(indirects);
}

bool CommandListRecorder::isInsideBeginEnd() const
{
  return tokenBeginStart >= 0;
}


} // namespace gl
