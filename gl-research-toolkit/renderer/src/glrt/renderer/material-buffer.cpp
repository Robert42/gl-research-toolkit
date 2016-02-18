#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>

namespace glrt {
namespace renderer {

using glrt::scene::resources::Material;

MaterialBuffer::MaterialBuffer(Type type)
  : type(type)
{
  blockSize = BlockSize::forType(type);
}

MaterialBuffer::~MaterialBuffer()
{
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
    // #ISSUE-63
    Q_UNREACHABLE();
  case Type::TEXTURED_OPAQUE:
    // #ISSUE-63
    Q_UNREACHABLE();
  case Type::TEXTURED_TRANSPARENT:
    // #ISSUE-63
    Q_UNREACHABLE();
  default:
    Q_UNREACHABLE();
  }
}




void MaterialBuffer::Initializer::begin(int expectedNumberMaterials, Type type)
{
  blockSize = BlockSize::forType(type);

  data.clear();
  data.reserve(expectedNumberMaterials*blockSize.blockOffset);
}

void MaterialBuffer::Initializer::append(const Material& material)
{
  if(data.capacity() < data.length() + blockSize.blockOffset)
    data.reserve(data.capacity() + blockSize.blockOffset*64);

  data.append_by_memcpy(material.data(), blockSize.dataSize);
  data.resize(data.length() + blockSize.blockOffset);
}

MaterialBuffer&& MaterialBuffer::Initializer::end()
{
  MaterialBuffer buffer(type);
  buffer.buffer = std::move(gl::Buffer(data.length(), gl::Buffer::UsageFlag::IMMUTABLE, data.data()));

  return std::move(buffer);
}

} // namespace renderer
} // namespace glrt
