#include <glrt/renderer/gl/status-capture.h>

namespace glrt {
namespace renderer {
namespace gl {

StatusCapture::StatusCapture()
  : _stateCapture(0)
{
}

StatusCapture::~StatusCapture()
{
  glDeleteStatesNV(1, &_stateCapture);
}

StatusCapture&& StatusCapture::capture()
{
  StatusCapture capture;

  glCreateStatesNV(1, &capture._stateCapture);
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
} // namespace renderer
} // namespace glrt
