#include <glrt/renderer/gl/program.h>
#include <glhelper/shaderobject.hpp>

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

void Program::use()
{
  GL_CALL(glUseProgram, programId);
}

void Program::useNone()
{
  GL_CALL(glUseProgram, 0);
}

void Program::loadFromBinary(const QByteArray& binary, GLenum binaryFormat)
{
  create();

  Q_ASSERT(programId != 0);

  GL_CALL(glProgramBinary, programId, binaryFormat, binary.data(), binary.length());
}

void Program::saveToBinary(QByteArray& binary, GLenum& binaryFormat)
{
  Q_ASSERT(programId != 0);

  GLint binaryLength = 0;
  GL_CALL(glGetProgramiv, programId, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

  Q_ASSERT(binaryLength != 0);

  binary.resize(binaryLength);

  GLsizei realLength;
  GL_CALL(glGetProgramBinary, binaryLength, &realLength, &binaryFormat, binary.data());

  Q_ASSERT(realLength == binaryLength);
}

void Program::loadFromFile(const QString& file)
{
}

void Program::saveToFile(const QString& file)
{
}

void Program::create()
{
  // delete the old program;
  gl::Program old(std::move(*this));
  Q_UNUSED(old);

  programId = GL_RET_CALL_NO_ARGS(glCreateProgram);
}


} // namespace gl
