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


void CommandListRecorder::beginTokenListWithCopy(const glm::ivec2& src)
{
  Q_ASSERT(!isInsideBeginEnd());

  tokenBeginStart = commandTokens.length();

  if(src[1] == tokenBeginStart)
  {
    tokenBeginStart = src[0];
  }else
  {
    Q_ASSERT(src[0]>=0);
    Q_ASSERT(src[1]>=0);
    Q_ASSERT(src[0]<=src[1]);
    Q_ASSERT(src[1]<commandTokens.length());
    // #BUG: commandTokens.data() is passed and within append_by_memcpy, a new capacty is chosen, which invalidates the passed pointer
    commandTokens.append_by_memcpy(commandTokens.data() + src[0], static_cast<size_t>(src[1] - src[0]));
  }
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

void CommandListRecorder::append_token_DrawElements(GLuint count, GLuint firstIndex, GLuint baseVertex, Strip strip)
{
  DrawElementsCommandNV token;
  token.count = count;
  token.firstIndex = firstIndex;
  token.baseVertex = baseVertex;
  append_token(&token, strip==Strip::NO_STRIP ? GL_DRAW_ELEMENTS_COMMAND_NV : GL_DRAW_ELEMENTS_STRIP_COMMAND_NV);
}

void CommandListRecorder::append_token_DrawArrays(GLuint count, GLuint first, Strip strip)
{
  DrawArraysCommandNV token;
  token.count = count;
  token.first = first;
  append_token(&token, strip==Strip::NO_STRIP ? GL_DRAW_ARRAYS_COMMAND_NV : GL_DRAW_ARRAYS_STRIP_COMMAND_NV);
}

void CommandListRecorder::append_token_DrawElementsInstanced(GLuint mode, GLuint count, GLuint instanceCount, GLuint firstIndex, GLuint baseVertex, GLuint baseInstance)
{
  DrawElementsInstancedCommandNV token;
  token.mode = mode;
  token.count = count;
  token.instanceCount = instanceCount;
  token.firstIndex = firstIndex;
  token.baseVertex = baseVertex;
  token.baseInstance = baseInstance;
  append_token(&token, GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV);
}

void CommandListRecorder::append_token_DrawArraysInstanced(GLuint mode, GLuint count, GLuint instanceCount, GLuint first, GLuint baseInstance)
{
  DrawArraysInstancedCommandNV token;
  token.mode = mode;
  token.count = count;
  token.instanceCount = instanceCount;
  token.first = first;
  token.baseInstance = baseInstance;
  append_token(&token, GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV);
}

void CommandListRecorder::append_token_ElementAddress(GLuint64 gpuAddress, GLuint typeSizeInByte)
{
  ElementAddressCommandNV token;
  token.addressLo = addressLo(gpuAddress);
  token.addressHi = addressHi(gpuAddress);
  token.typeSizeInByte = typeSizeInByte;
  append_token(&token, GL_ELEMENT_ADDRESS_COMMAND_NV);
}

void CommandListRecorder::append_token_AttributeAddress(GLuint index, GLuint64 gpuAddress)
{
  AttributeAddressCommandNV token;
  token.index = index;
  token.addressLo = addressLo(gpuAddress);
  token.addressHi = addressHi(gpuAddress);
  append_token(&token, GL_ATTRIBUTE_ADDRESS_COMMAND_NV);
}

void CommandListRecorder::append_token_UniformAddress(GLushort index, gl::ShaderObject::ShaderType shaderType, GLuint64 gpuAddress)
{
  UniformAddressCommandNV token;
  token.index = index;
  token.stage = glGetStageIndexNV(gl::ShaderObject::getGLShaderType(shaderType));
  token.addressLo = addressLo(gpuAddress);
  token.addressHi = addressHi(gpuAddress);
  append_token(&token, GL_UNIFORM_ADDRESS_COMMAND_NV);
}

void CommandListRecorder::append_token_BlendColor(const glm::vec4& color)
{
  BlendColorCommandNV token;
  token.red = color.r;
  token.green = color.g;
  token.blue = color.b;
  token.alpha = color.a;
  append_token(&token, GL_BLEND_COLOR_COMMAND_NV);
}

void CommandListRecorder::append_token_StencilRef(GLuint front, GLuint back)
{
  StencilRefCommandNV token;
  token.frontStencilRef = front;
  token.backStencilRef = back;
  append_token(&token, GL_STENCIL_REF_COMMAND_NV);
}

void CommandListRecorder::append_token_LineWidth(float lineWidth)
{
  LineWidthCommandNV token;
  token.lineWidth = lineWidth;
  append_token(&token, GL_LINE_WIDTH_COMMAND_NV);
}

void CommandListRecorder::append_token_PolygonOffset(float scale, float bias)
{
  PolygonOffsetCommandNV token;
  token.scale = scale;
  token.bias = bias;
  append_token(&token, GL_POLYGON_OFFSET_COMMAND_NV);
}

void CommandListRecorder::append_token_AlphaRef(float alphaRef)
{
  AlphaRefCommandNV token;
  token.alphaRef = alphaRef;
  append_token(&token, GL_ALPHA_REF_COMMAND_NV);
}

void CommandListRecorder::append_token_Viewport(const glm::uvec2& pos, const glm::uvec2& size)
{
  ViewportCommandNV token;
  token.x = pos.x;
  token.y = pos.y;
  token.width = size.x;
  token.height = size.y;
  append_token(&token, GL_VIEWPORT_COMMAND_NV);
}

void CommandListRecorder::append_token_Scissor(const glm::uvec2& pos, const glm::uvec2& size)
{
  ScissorCommandNV token;
  token.x = pos.x;
  token.y = pos.y;
  token.width = size.x;
  token.height = size.y;
  append_token(&token, GL_SCISSOR_COMMAND_NV);
}

void CommandListRecorder::append_token_FrontFace(FrontFace frontFace)
{
  FrontFaceCommandNV token;
  token.frontFace = frontFace == FrontFace::CW ? 0 : 1;
  append_token(&token, GL_FRONTFACE_COMMAND_NV);
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
