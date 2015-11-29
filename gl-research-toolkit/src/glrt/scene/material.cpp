#include <glrt/scene/material.h>

namespace glrt {
namespace scene {


// ======== Material ===========================================================


Material::Material(gl::Buffer&& buffer)
  : uniformBuffer(std::move(buffer))
{

}


// ======== PlainColorMaterial =================================================


PlainColorMaterial::PlainColorMaterial(const UniformData& data)
  : Material(gl::Buffer(sizeof(UniformData), gl::Buffer::UsageFlag::IMMUTABLE, &data))
{
}


} // namespace scene
} // namespace glrt

