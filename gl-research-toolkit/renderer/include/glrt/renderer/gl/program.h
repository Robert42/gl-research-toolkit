#ifndef GL_PROGRAM_H
#define GL_PROGRAM_H

#include <glrt/renderer/dependencies.h>
#include <GL/glew.h>

namespace gl {

class Program
{
  Q_DISABLE_COPY(Program)
public:
  GLuint programId = 0;

  Program();
  ~Program();

  Program(Program&& program);
  void operator=(Program&& program);

  void use();
  static void useNone();

  void loadFromBinary(const QByteArray& binary, GLenum binaryFormat);
  void saveToBinary(QByteArray& binary, GLenum& binaryFormat);
  void loadFromFile(const QString& file);
  void saveToFile(const QString& filename);

private:
  void create();
  static quint64 magicNumber();
};

} // namespace gl

#endif // GL_PROGRAM_H
