#include <glrt/renderer/gl/command-list.h>

namespace glrt {
namespace renderer {
namespace gl {

CommandList::CommandList()
  : _commandListHandler(0)
{
}

CommandList::~CommandList()
{
  glDeleteCommandListsNV(1, &_commandListHandler);
}

CommandList&& CommandList::create()
{
  CommandList commandList;

  glCreateCommandListsNV(1, &commandList._commandListHandler);

  return std::move(commandList);
}

CommandList::CommandList(CommandList&& other)
  : _commandListHandler(other._commandListHandler)
{
  other._commandListHandler = 0;
}

CommandList& CommandList::operator=(CommandList&& other)
{
  std::swap(other._commandListHandler, this->_commandListHandler);
  return *this;
}

void CommandList::call()
{
  glCallCommandListNV(_commandListHandler);
}

} // namespace gl
} // namespace renderer
} // namespace glrt
