#include <glrt/renderer/gl/command-list.h>
#include <glrt/renderer/gl/status-capture.h>

namespace gl {

CommandList::CommandList()
{
}

CommandList::~CommandList()
{
  GL_CALL(glDeleteCommandListsNV, 1, &_commandListHandler);
}

CommandList&& CommandList::create(GLuint num_segments)
{
  CommandList commandList;

  GLuint& list = commandList._commandListHandler;

  GL_CALL(glCreateCommandListsNV, 1, &list);

  GL_CALL(glCommandListSegmentsNV, list, num_segments);

  commandList.num_segments = num_segments;

  return std::move(commandList);
}

CommandList::CommandList(CommandList&& other)
  : _commandListHandler(other._commandListHandler),
    num_segments(other.num_segments)
{
  other._commandListHandler = 0;
  other.num_segments = 0;
}

CommandList& CommandList::operator=(CommandList&& other)
{
  std::swap(other._commandListHandler, this->_commandListHandler);
  std::swap(other.num_segments, this->num_segments);
  return *this;
}



void CommandList::setSegment(GLuint segment,
                             const glrt::Array<const void*>& indirects,
                             const glrt::Array<GLsizei>& sizes,
                             const glrt::Array<const StatusCapture*>& states,
                             const glrt::Array<const FramebufferObject*>& fbos)
{
  Q_ASSERT(segment < this->num_segments);

  GLuint count = GLuint(indirects.length());
  Q_ASSERT(count == GLuint(sizes.length()));
  Q_ASSERT(count == GLuint(states.length()));
  Q_ASSERT(count == GLuint(fbos.length()));

  glrt::Array<GLuint> states_array;
  glrt::Array<GLuint> fbos_array;

  states_array.reserve(int(count));
  fbos_array.reserve(int(count));

  for(GLuint i=0; i<count; ++i)
  {
    states_array.append(states[i]->glhandle());
    fbos_array.append(fbos[i]->GetInternHandle());
  }

  GL_CALL(glListDrawCommandsStatesClientNV, _commandListHandler, segment, const_cast<const void**>(indirects.data()), sizes.data(), states_array.data(), fbos_array.data(), count);
}

void CommandList::call()
{
  glCallCommandListNV(_commandListHandler);
}

} // namespace gl
