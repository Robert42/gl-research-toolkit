#include <glrt/scene/resources/texture-manager.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace scene {
namespace resources {

TextureManager::TextureManager()
{
}

TextureManager* TextureManager::instance()
{
  return &ResourceManager::instance()->textureManager;
}


} // namespace resources
} // namespace scene
} // namespace glrt
