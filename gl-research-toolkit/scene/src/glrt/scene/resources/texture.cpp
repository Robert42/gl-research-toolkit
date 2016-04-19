#include <glrt/scene/resources/texture.h>
#include <glrt/scene/resources/texture-file.h>

namespace glrt {
namespace scene {
namespace resources {

Texture::Texture()
{

}

QDebug operator<<(QDebug d, const Texture& t)
{
  return d << "Texture("<<t.file.absoluteFilePath()<<")";
}

GLuint Texture::load()
{
  return glrt::scene::resources::TextureFile::loadFromFile(this->file);
}

} // namespace resources
} // namespace scene
} // namespace glrt
