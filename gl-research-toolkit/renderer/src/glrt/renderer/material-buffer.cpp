#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>

namespace glrt {
namespace renderer {

using glrt::scene::resources::Material;

MaterialBuffer::MaterialBuffer(Type type)
  : type(type),
    buffer(16,gl::Buffer::MAP_WRITE)
{
  switch(type)
  {
  case Type::PLAIN_COLOR:
    initBlockOffset<Material::PlainColor>();
    break;
  case Type::TEXTURED_MASKED:
    // #TODO
  case Type::TEXTURED_OPAQUE:
    // #TODO
  case Type::TEXTURED_TRANSPARENT:
    // #TODO
  default:
    Q_UNREACHABLE();
  }
}

void MaterialBuffer::bind(int i)
{
  buffer.BindUniformBuffer(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, i*blockOffset, dataSize);
}


template<typename T>
void MaterialBuffer::initBlockOffset()
{
  dataSize = sizeof(T);
  blockOffset = aligned_vector<T>::retrieveAlignmentOffset(aligned_vector<T>::Alignment::UniformBufferOffsetAlignment);
}

} // namespace renderer
} // namespace glrt
