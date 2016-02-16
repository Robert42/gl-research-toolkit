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
  enum class Mode
  {
    TRIANGLES = GL_TRIANGLES,
  };

  StatusCapture();
  ~StatusCapture();

  static StatusCapture&& capture(Mode mode);

  StatusCapture(StatusCapture&& other);
  StatusCapture& operator=(StatusCapture&& other);

  GLuint glhandle() const{return _stateCapture;}

private:
  GLuint _stateCapture;
};

} // namespace gl
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_GL_STATUSCAPTURE_H
