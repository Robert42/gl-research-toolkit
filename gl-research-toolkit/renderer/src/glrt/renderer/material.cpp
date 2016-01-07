#include <glrt/renderer/material.h>

namespace glrt {
namespace renderer {


// ======== Material ===========================================================


MaterialInstance::MaterialInstance(const Type type, gl::Buffer&& buffer)
  : type(type),
    uniformBuffer(std::move(buffer))
{

}

MaterialInstance::Ptr MaterialInstance::createDummyMaterial()
{
  PlainColorMaterial::UniformData data;

  data.base_color = glm::vec3(0);
  data.emission = glm::vec3(1, 0, 1);

  return Ptr(new PlainColorMaterial(data));
}


// ======== PlainColorMaterial =================================================


PlainColorMaterial::PlainColorMaterial(const UniformData& data)
  : MaterialInstance(Type::PLAIN_COLOR,
                     gl::Buffer(sizeof(UniformData), gl::Buffer::UsageFlag::IMMUTABLE, &data))
{
}


} // namespace renderer
} // namespace glrt
