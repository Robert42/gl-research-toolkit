#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace renderer {

using glrt::scene::resources::Material;

MaterialBuffer::MaterialBuffer()
{
}

MaterialBuffer::MaterialBuffer(MaterialBuffer&& other)
  : buffer(std::move(other.buffer))
{
}

MaterialBuffer::~MaterialBuffer()
{
}

MaterialBuffer& MaterialBuffer::operator=(MaterialBuffer&& other)
{
  this->buffer = std::move(other.buffer);
  return *this;
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
  switch(type)
  {
  case Type::PLAIN_COLOR:
    return forType<Material::PlainColor>();
  case Type::TEXTURED_MASKED:
  case Type::TEXTURED_OPAQUE:
  case Type::TEXTURED_TRANSPARENT:
    return forType<Material::Textured>();
  default:
    Q_UNREACHABLE();
  }
}

int MaterialBuffer::BlockSize::expectedBlockOffset()
{
  static_assert(sizeof(Material::PlainColor) <= sizeof(Material::Textured), "The expected offset should be the largest data struct");

#ifdef QT_DEBUG
  if(forType<Material::PlainColor>().blockOffset != forType<Material::Textured>().blockOffset)
    qWarning() << "Warning: different block offset for material blocks.\n"
                  "    PlainColor:" <<  forType<Material::PlainColor>().blockOffset << "\n    Textured: " << forType<Material::Textured>().blockOffset << "\n";
#endif

  return forType<Material::Textured>().blockOffset;
}

MaterialBuffer::Initializer::Initializer(gl::CommandListRecorder& recorder, const glrt::scene::resources::ResourceManager& resourceManager)
  : recorder(recorder),
    resourceManager(resourceManager)
{
}

void MaterialBuffer::Initializer::begin(int expectedNumberMaterials)
{
  data.clear();
  data.reserve(expectedNumberMaterials * BlockSize::expectedBlockOffset());
}

int MaterialBuffer::Initializer::append(const Uuid<Material>& materialUuid)
{
  Material material = resourceManager.materialForUuid(materialUuid);

  BlockSize blockSize = BlockSize::forType(material.type);

  if(data.capacity() < data.length() + blockSize.blockOffset)
    data.reserve(data.capacity() + blockSize.blockOffset*64);

  int indexOfCurrentUniformBlock = data.length();
  data.reserve(data.length() + blockSize.blockOffset);
  data.append_by_memcpy(material.data(), blockSize.dataSize);
  data.resize(data.length() + blockSize.blockOffset);

  gpuAddresses[materialUuid] = indexOfCurrentUniformBlock;

  return indexOfCurrentUniformBlock;
}

MaterialBuffer MaterialBuffer::Initializer::end()
{
  gl::Buffer buffer(data.length(), gl::Buffer::UsageFlag::IMMUTABLE, data.data());

  GLuint64 baseAddress = buffer.gpuBufferAddress();
  for(GLuint64& address : gpuAddresses)
    address += baseAddress;

  MaterialBuffer materialBuffer;
  materialBuffer.buffer = std::move(buffer);
  return std::move(materialBuffer);
}

} // namespace renderer
} // namespace glrt
