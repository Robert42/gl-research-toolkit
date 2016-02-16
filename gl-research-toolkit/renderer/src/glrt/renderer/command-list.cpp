#include "command-list.h"

namespace glrt {
namespace renderer {

CommandList::CommandList()
  : _commandListHandler(0)
{
}

CommandList::~CommandList()
{
  glDeleteCommandListsNV(1, &_commandListHandler);
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

} // namespace renderer
} // namespace glrt
