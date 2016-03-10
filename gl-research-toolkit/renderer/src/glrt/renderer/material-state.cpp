#include <glrt/renderer/material-state.h>

namespace glrt {
namespace renderer {


MaterialState::MaterialState(MaterialState&& other)
  : stateCapture(std::move(other.stateCapture)),
    shader(std::move(other.shader))
{
}

MaterialState& MaterialState::operator=(MaterialState&& other)
{
  stateCapture = std::move(other.stateCapture);
  shader = std::move(other.shader);
  return *this;
}

MaterialState::MaterialState(int shader)
  : shader(shader)
{
}

MaterialState::~MaterialState()
{
}


} // namespace renderer
} // namespace glrt
