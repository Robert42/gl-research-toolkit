#ifndef GLRT_SHADER_SHADERFACTORY_H
#define GLRT_SHADER_SHADERFACTORY_H

#include <glhelper/shaderobject.hpp>

namespace glrt {
namespace shader {

class ShaderFactory
{
public:
  ShaderFactory(const ShaderFactory&) = delete;
  ShaderFactory(ShaderFactory&&) = delete;
  ShaderFactory& operator=(const ShaderFactory&) = delete;
  ShaderFactory& operator=(ShaderFactory&&) = delete;

  ShaderFactory();
  virtual ~ShaderFactory();

  virtual gl::ShaderObject* create() = 0;
};

} // namespace shader
} // namespace glrt

#endif // GLRT_SHADER_SHADERFACTORY_H
