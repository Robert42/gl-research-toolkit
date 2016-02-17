#ifndef GL_STATUSCAPTURE_H
#define GL_STATUSCAPTURE_H

#include <glrt/dependencies.h>
#include <nvcommandlist.h>

namespace gl {

class StatusCapture final
{
  Q_DISABLE_COPY(StatusCapture)

public:
  enum class Mode : GLenum
  {
    TRIANGLES = GL_TRIANGLES,
  };

  StatusCapture();
  ~StatusCapture();

  static StatusCapture capture(Mode mode);

  StatusCapture(StatusCapture&& other);
  StatusCapture& operator=(StatusCapture&& other);

  GLuint glhandle() const{return _stateCapture;}

private:
  GLuint _stateCapture;
};

} // namespace gl

#endif // GL_STATUSCAPTURE_H
