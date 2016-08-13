#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace renderer {

using glrt::scene::resources::Material;

MaterialBuffer::MaterialBuffer(scene::resources::ResourceManager& resourceManager, quint16 capacity)
  : resourceManager(resourceManager),
    blockOffset(static_cast<GLuint64>(BlockSize::expectedBlockOffset())),
    totalCapacity(blockOffset*capacity),
    buffer(gl::Buffer(static_cast<GLintptr>(totalCapacity), gl::Buffer::UsageFlag::MAP_WRITE))
{
}

MaterialBuffer::~MaterialBuffer()
{
}

void MaterialBuffer::map(quint16 maxNumMaterials)
{
  mapped_data = reinterpret_cast<byte*>(buffer.Map(0, static_cast<GLsizeiptr>(blockOffset*maxNumMaterials), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
}

Material::Type MaterialBuffer::append(const Uuid<MaterialBuffer::Material>& materialUuid)
{
  resourceManager.prepareForGpuBuffer(materialUuid);
  const Material material = resourceManager.materialForUuid(materialUuid);

  BlockSize blockSize = BlockSize::forType(material.type);
  std::memcpy(mapped_data, material.data(), static_cast<size_t>(blockSize.dataSize));

  mapped_data += blockOffset;

  return material.type;
}

void MaterialBuffer::unmap()
{
  buffer.Unmap();
  mapped_data = nullptr;
}

template<typename T>
MaterialBuffer::BlockSize MaterialBuffer::BlockSize::forType()
{
  BlockSize blockSize;
  blockSize.dataSize = sizeof(T);
  blockSize.blockOffset = aligned_vector<T>::retrieveAlignmentOffset(aligned_vector<T>::Alignment::UniformBufferOffsetAlignment);
  return blockSize;
}

MaterialBuffer::BlockSize MaterialBuffer::BlockSize::forType(Type type)
{
  if(type.testFlag(TypeFlag::TEXTURED))
    return forType<Material::Textured<GLuint64>>();
  else if(type.testFlag(TypeFlag::PLAIN_COLOR))
    return forType<Material::PlainColor>();
  Q_UNREACHABLE();
}

int MaterialBuffer::BlockSize::expectedBlockOffset()
{
  static_assert(sizeof(Material::PlainColor) <= sizeof(Material::Textured<GLuint64>), "The expected offset should be the largest data struct");

#ifdef QT_DEBUG
  if(forType<Material::PlainColor>().blockOffset != forType<Material::Textured<GLuint64>>().blockOffset)
    qWarning() << "Warning: different block offset for material blocks.\n"
                  "    PlainColor:" <<  forType<Material::PlainColor>().blockOffset << "\n    Textured: " << forType<Material::Textured<GLuint64>>().blockOffset << "\n";
#endif

  return forType<Material::Textured<GLuint64>>().blockOffset;
}

} // namespace renderer
} // namespace glrt
