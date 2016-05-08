#include <glrt/renderer/gl/program.h>
#include <glhelper/gl.hpp>

namespace gl {


Program::Program()
{
}

Program::~Program()
{
  GL_CALL(glDeleteProgram, programId);
}

Program::Program(Program&& program)
  : programId(program.programId)
{
  program.programId = 0;
}

void Program::operator=(Program&& program)
{
  std::swap(program.programId, this->programId);
}


} // namespace gl
