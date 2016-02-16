#ifndef GLRT_RENDERER_GL_STATUSCAPTURE_H
#define GLRT_RENDERER_GL_STATUSCAPTURE_H

#include <glrt/dependencies.h>
#include <nvcommandlist.h>

namespace glrt {
namespace renderer {
namespace gl {

class StatusCapture
{
  Q_DISABLE_COPY(StatusCapture)

public:
  StatusCapture();
  ~StatusCapture();

  static StatusCapture&& capture();

  StatusCapture(StatusCapture&& other);
  StatusCapture& operator=(StatusCapture&& other);

private:
  GLuint _stateCapture;
};

} // namespace gl
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_GL_STATUSCAPTURE_H
