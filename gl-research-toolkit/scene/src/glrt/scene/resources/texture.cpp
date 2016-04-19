#include <glrt/scene/resources/texture.h>
#include <glrt/scene/resources/texture-file.h>

namespace glrt {
namespace scene {
namespace resources {

Texture::Texture()
{

}

bool Texture::isEmpty() const
{
  return _file.filePath().isEmpty();
}

QDebug operator<<(QDebug d, const Texture& t)
{
  return d << "Texture("<<t._file.absoluteFilePath()<<")";
}

GLuint Texture::load()
{
  return glrt::scene::resources::TextureFile::loadFromFile(this->_file);
}

void Texture::setFile(const QFileInfo& file)
{
  _file = file;
}

} // namespace resources
} // namespace scene
} // namespace glrt
