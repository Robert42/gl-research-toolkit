#ifndef GLRT_SCENE_RESOURCES_TEXTUREMANAGER_H
#define GLRT_SCENE_RESOURCES_TEXTUREMANAGER_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/uuid.h>

namespace glrt {
namespace scene {
namespace resources {

class Texture;
class TextureSampler;
class TextureManager : public QObject
{
  Q_OBJECT
public:
  TextureManager();

  static TextureManager* instance();

  struct TextureHandle
  {
    int textureId;
    int samplerId;
  };

  virtual TextureHandle handleFor(const Uuid<Texture>& texture) = 0;
  virtual TextureHandle handleFor(const Uuid<Texture>& texture, const TextureSampler& sampler) = 0;
  virtual quint64 gpuHandle(TextureHandle handle) = 0;

  // #TODO: vom static mesh manager abschauen?

  // #TODO: clear, when a scene was laoded?
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTUREMANAGER_H
