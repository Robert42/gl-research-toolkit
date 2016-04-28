#ifndef GLRT_RENDERER_GLTEXTUREMANAGER_H
#define GLRT_RENDERER_GLTEXTUREMANAGER_H

#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/texture-manager.h>
#include <glrt/scene/resources/texture-file.h>
#include <glrt/renderer/declarations.h>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {

class GlTextureManager final : public TextureManager
{
public:
  glrt::scene::resources::ResourceManager* resourceManager;

  GlTextureManager(glrt::scene::resources::ResourceManager* resourceManager);
  ~GlTextureManager();

  void removeUnusedTextures(QSet<Uuid<Texture>> usedTextures) override;
  Uuid<Texture> textureUuidForHandle(const TextureHandle& handle) override;
  Uuid<Texture> textureUuidForGpuPtr(quint64 handle) override;

  TextureHandle handleFor(const Uuid<Texture>& texture) override;
  TextureHandle handleFor(const Uuid<Texture>& texture, const TextureSampler& sampler) override;

  quint64 gpuHandle(TextureHandle handle) override;


private:
  QVector<Uuid<Texture>> textureIds;
  QVector<TextureSampler> textureSamplerIds;

  QVector<GLuint> textures;

  QHash<GLuint64, Uuid<Texture>> _textureUuidForGpuPtr;

  QSet<GLuint64> residentHandles;

  int registerSampler(const TextureSampler& sampler);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_GLTEXTUREMANAGER_H
