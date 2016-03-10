#include <glrt/renderer/material-state.h>

namespace glrt {
namespace renderer {


MaterialState::MaterialState(MaterialState&& other)
  : stateCapture(std::move(other.stateCapture)),
    shader(std::move(other.shader)),
    flags(std::move(other.flags))
{
}

MaterialState& MaterialState::operator=(MaterialState&& other)
{
  stateCapture = std::move(other.stateCapture);
  std::swap(shader, other.shader);
  std::swap(flags, other.flags);
  return *this;
}

MaterialState::MaterialState(int shader, Flags flags)
  : shader(shader),
    flags(flags)
{
}

MaterialState::~MaterialState()
{
}

void MaterialState::activateStateForFlags()
{
  glDepthFunc(GL_LEQUAL); // necessary for a depth prepass
  if(hasFlag(Flags::DEPTH_TEST))
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
  glDepthMask(glHasFlag(Flags::DEPTH_WRITE));
  glColorMask(glHasFlag(Flags::COLOR_WRITE), glHasFlag(Flags::COLOR_WRITE), glHasFlag(Flags::COLOR_WRITE), glHasFlag(Flags::ALPHA_WRITE));
}

void MaterialState::deactivateStateForFlags()
{
  glDepthFunc(GL_LESS);
  glDisable(GL_DEPTH_TEST);

  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

bool MaterialState::hasFlag(Flags flag) const
{
  return (this->flags & flag) != Flags::NONE;
}

GLboolean MaterialState::glHasFlag(Flags flag) const
{
  return hasFlag(flag) ? GL_TRUE : GL_FALSE;
}


} // namespace renderer
} // namespace glrt
