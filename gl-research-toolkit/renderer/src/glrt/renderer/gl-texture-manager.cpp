#include <glrt/renderer/gl-texture-manager.h>
#include <glrt/scene/resources/texture-file.h>
#include <glrt/scene/resources/resource-index.h>

namespace glrt {
namespace renderer {


GlTextureManager::GlTextureManager(scene::resources::ResourceManager* resourceManager)
  : resourceManager(resourceManager)
{
}


void GlTextureManager::removeUnusedTextures(QSet<Uuid<Texture>> usedTextures)
{
  usedTextures |= QSet<Uuid<Texture>>({scene::resources::uuids::blackTexture, scene::resources::uuids::fallbackDiffuseTexture, scene::resources::uuids::fallbackNormalTexture, scene::resources::uuids::fallbackSRMOTexture});

  QSet<GLuint64> copy = residentHandles;
  for(GLuint64 gpuPtr : copy)
  {
    Q_ASSERT(_textureUuidForGpuPtr.contains(gpuPtr));
    if(!_textureUuidForGpuPtr.contains(gpuPtr))
      continue;

    Uuid<Texture> texture = _textureUuidForGpuPtr.value(gpuPtr);

    if(usedTextures.contains(texture))
      continue;

    residentHandles.remove(gpuPtr);

    GL_CALL(glMakeTextureHandleNonResidentNV, gpuPtr);
  }
}

Uuid<Texture> GlTextureManager::textureUuidForHandle(const TextureHandle& handle)
{
  if(handle.textureId < textureIds.length())
    return textureIds[handle.textureId];
  return scene::resources::uuids::blackTexture;
}

Uuid<Texture> GlTextureManager::textureUuidForGpuPtr(quint64 handle)
{
  if(_textureUuidForGpuPtr.contains(handle))
    return _textureUuidForGpuPtr[handle];
  return scene::resources::uuids::blackTexture;
}


GlTextureManager::~GlTextureManager()
{
  GL_CALL(glDeleteTextures,textures.length(), textures.data());
}


GlTextureManager::TextureHandle GlTextureManager::handleFor(const Uuid<Texture>& texture)
{
  return handleFor(texture, resourceManager->defaultTextureSamplerForTextureUuid(texture));
}

GlTextureManager::TextureHandle GlTextureManager::handleFor(const Uuid<Texture>& texture, const TextureSampler& sampler)
{
  int textureId = textureIds.indexOf(texture);
  int textureSamplerId = textureSamplerIds.indexOf(sampler);

  if(textureId < 0)
  {
    textureIds.append(texture);
    textureId = textureIds.length()-1;
  }

  if(textureSamplerId < 0)
    textureSamplerId = registerSampler(sampler);

  Q_ASSERT(textureId >= 0);
  Q_ASSERT(textureSamplerId >= 0);

  GlTextureManager::TextureHandle handle;
  handle.textureId = textureId;
  handle.samplerId = textureSamplerId;

  return handle;
}

quint64 GlTextureManager::gpuHandle(TextureHandle handle)
{
  Q_ASSERT(handle.textureId>=0 && handle.textureId < textureIds.length());
  Q_ASSERT(handle.samplerId>=0 && handle.samplerId < textureSamplerIds.length());

  if(handle.textureId >= textures.length())
    textures.resize(handle.textureId+1);

  const gl::SamplerObject& samplerObject = gl::SamplerObject::GetSamplerObject(textureSamplerIds[handle.samplerId].description);
  GLuint& texture = textures[handle.textureId];

  if(!texture)
    texture = glrt::scene::resources::TextureFile::loadFromFile(resourceManager->textureForUuid(textureIds[handle.textureId]).file);

  GLuint sampler = samplerObject.GetInternHandle();

  GLuint64 ptr = GL_RET_CALL(glGetTextureSamplerHandleNV, texture, sampler);

  if(!_textureUuidForGpuPtr.contains(ptr))
    _textureUuidForGpuPtr.insert(ptr, textureUuidForHandle(handle));

  if(!residentHandles.contains(ptr))
  {
    GL_CALL(glMakeTextureHandleResidentNV, ptr);
    residentHandles.insert(ptr);
  }

  return ptr;
}

int GlTextureManager::registerSampler(const TextureSampler& sampler)
{
  int index = textureSamplerIds.indexOf(sampler);

  if(index < 0)
  {
    textureSamplerIds.append(sampler);
    index = textureSamplerIds.indexOf(sampler);
    Q_ASSERT(index >= 0);
  }

  return index;
}

} // namespace Renderer
} // namespace glrt
