#include <glrt/scene/resources/texture.h>

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

} // namespace resources
} // namespace scene
} // namespace glrt
