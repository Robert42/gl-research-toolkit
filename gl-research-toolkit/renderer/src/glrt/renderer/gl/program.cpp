#include <glrt/renderer/gl/program.h>
#include <glrt/toolkit/plain-old-data-stream.h>
#include <glhelper/shaderobject.hpp>

namespace gl {

using namespace glrt;

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

void Program::loadFromBinary(const QByteArray& binary)
{
  create();

  Q_ASSERT(programId != 0);

  GLenum binaryFormat = *reinterpret_cast<const GLenum*>(binary.data());
  const void* data = binary.data() + sizeof(GLenum);
  int numBytes = binary.length() - int(sizeof(GLenum));

  GL_CALL(glProgramBinary, programId, binaryFormat, data, numBytes);
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
  GL_CALL(glGetProgramBinary, programId, binaryLength, &realLength, &binaryFormat, binary.data());

  Q_ASSERT(realLength == binaryLength);
}

void Program::loadFromFile(const QString& filename)
{
  QFile file(filename);

  if(!file.open(QFile::ReadOnly))
    throw GLRT_EXCEPTION(QString("Couldn't open the files <%0> for reading a binary shader.").arg(filename));

  QByteArray binary;
  GLenum binaryFormat;

  if(readValue<quint64>(file) != magicNumber())
    throw GLRT_EXCEPTION(QString("Wrong magic number in binary shader file.").arg(filename));

  binaryFormat = readValue<GLenum>(file);
  binary = file.readAll();

  loadFromBinary(binary, binaryFormat);
}

void Program::saveToFile(const QString& filename)
{
  QByteArray binary;
  GLenum binaryFormat;

  saveToBinary(binary, binaryFormat);
  saveBinaryDataToFile(filename, binary, binaryFormat);
}

void Program::saveBinaryDataToFile(const QString& filename, const QByteArray& binary, GLenum binaryFormat)
{
  QFile file(filename);

  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Couldn't open the files <%0> for writing a binary shader.").arg(filename));

  writeValue(file, magicNumber());
  writeValue(file, binaryFormat);
  file.write(binary);
}

QByteArray Program::saveShaderObjectToByteArray(gl::ShaderObject* shaderObject)
{
  QByteArray binary;
  GLenum binaryFormat;

  std::vector<char> tmpData = shaderObject->GetProgramBinary(binaryFormat);
  int dataLength = int(tmpData.size());

  Q_ASSERT(tmpData.size() < std::numeric_limits<int>::max());

  binary.append(reinterpret_cast<char*>(&binaryFormat), int(sizeof(binaryFormat)));
  binary.append(tmpData.data(), dataLength);

  return binary;
}

void Program::saveShaderObjectToFile(const QString& filename, gl::ShaderObject* shaderObject)
{
  QByteArray binary;
  GLenum binaryFormat;

  std::vector<char> tmpData = shaderObject->GetProgramBinary(binaryFormat);
  int dataLength = int(tmpData.size());

  Q_ASSERT(tmpData.size() < std::numeric_limits<int>::max());

  binary.append(tmpData.data(), dataLength);

  saveBinaryDataToFile(filename, binary, binaryFormat);
}

quint64 Program::magicNumber()
{
  return magicNumberForString("bin-prog");
}

void Program::create()
{
  // delete the old program;
  gl::Program old(std::move(*this));
  Q_UNUSED(old);

  Q_ASSERT(programId == 0);

  programId = GL_RET_CALL_NO_ARGS(glCreateProgram);
}


} // namespace gl
