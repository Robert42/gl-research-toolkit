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
    int textureId = 0;
    int samplerId = 0;
  };

  virtual void removeUnusedTextures(QSet<Uuid<Texture>> usedTextures) = 0;
  virtual Uuid<Texture> textureUuidForHandle(const TextureHandle& handle) = 0;
  virtual Uuid<Texture> textureUuidForGpuPtr(quint64 handle) = 0;

  virtual TextureHandle handleFor(const Uuid<Texture>& texture) = 0;
  virtual TextureHandle handleFor(const Uuid<Texture>& texture, const TextureSampler& sampler) = 0;
  virtual quint64 gpuHandle(TextureHandle handle) = 0;
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTUREMANAGER_H
