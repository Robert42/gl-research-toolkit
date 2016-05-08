#include "managed-shader.h"

#include <glhelper/gl.hpp>

namespace glrt {
namespace renderer {

ManagedShader::ManagedShader()
  : _shaderObject(0)
{
}

ManagedShader::~ManagedShader()
{
  GL_CALL(glDeleteProgram, _shaderObject);
}

ManagedShader::ManagedShader(ManagedShader&& other)
  : _shaderObject(other._shaderObject)
{
  other._shaderObject = 0;
}

void ManagedShader::operator=(ManagedShader&& other)
{
  std::swap(other._shaderObject, this->_shaderObject);
}

} // namespace renderer
} // namespace glrt
