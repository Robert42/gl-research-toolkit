#include <glrt/renderer/gl/status-capture.h>
#include <glhelper/gl.hpp>

namespace gl {

StatusCapture::StatusCapture()
  : _stateCapture(0)
{
}

StatusCapture::~StatusCapture()
{
  GL_CALL(glDeleteStatesNV, 1, &_stateCapture);
}

StatusCapture StatusCapture::capture(Mode mode)
{
  StatusCapture capture;

  GL_CALL(glCreateStatesNV, 1, &capture._stateCapture);
  GL_CALL(glStateCaptureNV, capture._stateCapture, static_cast<GLenum>(mode));
  return std::move(capture);
}

StatusCapture::StatusCapture(StatusCapture&& other)
  : _stateCapture(other._stateCapture)
{
  other._stateCapture = 0;
}

StatusCapture& StatusCapture::operator=(StatusCapture&& other)
{
  std::swap(other._stateCapture, this->_stateCapture);
  return *this;
}

} // namespace gl
