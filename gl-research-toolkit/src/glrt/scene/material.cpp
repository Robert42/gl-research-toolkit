#include <glrt/scene/material.h>

namespace glrt {
namespace scene {


// ======== Material ===========================================================


MaterialInstance::MaterialInstance(const Type type, gl::Buffer&& buffer)
  : type(type),
    uniformBuffer(std::move(buffer))
{

}


// ======== PlainColorMaterial =================================================


PlainColorMaterial::PlainColorMaterial(const UniformData& data)
  : MaterialInstance(Type::PLAIN_COLOR,
                     gl::Buffer(sizeof(UniformData), gl::Buffer::UsageFlag::IMMUTABLE, &data))
{
}


} // namespace scene
} // namespace glrt

