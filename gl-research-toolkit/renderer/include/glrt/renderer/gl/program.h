#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H

#include <glrt/renderer/dependencies.h>
#include <GL/glew.h>

namespace gl {

class Program
{
  Q_DISABLE_COPY(Program)
public:
  Program();
  ~Program();

  Program(Program&& program);
  void operator=(Program&& program);

private:
  GLuint programId = 0;
};

} // namespace gl

#endif // GL_PROGRAM_H
