#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>

namespace glrt {
namespace renderer {

using glrt::scene::resources::Material;

MaterialBuffer::MaterialBuffer(Type type)
  : type(type),
    buffer(nullptr)
{
  switch(type)
  {
  case Type::PLAIN_COLOR:
    initBlockOffset<Material::PlainColor>();
    break;
  case Type::TEXTURED_MASKED:
    // #ISSUE-63
  case Type::TEXTURED_OPAQUE:
    // #ISSUE-63
  case Type::TEXTURED_TRANSPARENT:
    // #ISSUE-63
  default:
    Q_UNREACHABLE();
  }
}

MaterialBuffer::~MaterialBuffer()
{
  delete buffer;
}

void MaterialBuffer::bind(int i)
{
  Q_ASSERT(buffer != nullptr);
  buffer->BindUniformBuffer(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, i*blockOffset, dataSize);
}

void MaterialBuffer::clear()
{
  delete buffer;
  buffer = nullptr;
}


template<typename T>
void MaterialBuffer::initBlockOffset()
{
  dataSize = sizeof(T);
  blockOffset = aligned_vector<T>::retrieveAlignmentOffset(aligned_vector<T>::Alignment::UniformBufferOffsetAlignment);
}



MaterialBuffer::Initializer::Initializer(MaterialBuffer& buffer, int expectedNumberMaterials)
  : buffer(buffer),
    materialsAdded(0)
{
  data.reserve(expectedNumberMaterials*buffer.blockOffset);
}

MaterialBuffer::Initializer::~Initializer()
{
  buffer.clear();
  buffer.buffer = new gl::Buffer(data.length(), gl::Buffer::UsageFlag::IMMUTABLE, data.data());
}

void MaterialBuffer::Initializer::append(const Material& material)
{
  if(data.capacity() < data.length() + buffer.blockOffset)
    data.reserve(data.capacity() + buffer.blockOffset*64);

  void* targetLocation = data.data() + data.length();
  data.resize(data.length() + buffer.blockOffset);

  std::memcpy(targetLocation, material.data(), buffer.dataSize);
}

} // namespace renderer
} // namespace glrt
