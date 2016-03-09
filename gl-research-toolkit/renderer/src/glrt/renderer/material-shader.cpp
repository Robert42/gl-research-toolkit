#include <glrt/renderer/material-shader.h>

namespace glrt {
namespace renderer {


MaterialShader::MaterialShader(MaterialShader&& other)
  : stateCapture(std::move(other.stateCapture)),
    shader(std::move(other.shader))
{
}

MaterialShader& MaterialShader::operator=(MaterialShader&& other)
{
  stateCapture = std::move(other.stateCapture);
  shader = std::move(other.shader);
  return *this;
}

MaterialShader::MaterialShader(ReloadableShader&& shader)
  : shader(std::move(shader))
{
}

MaterialShader::MaterialShader(const QSet<QString>& preprocessorBlock)
  : MaterialShader(std::move(ReloadableShader("material",
                                              QDir(GLRT_SHADER_DIR"/materials"),
                                              preprocessorBlock)))
{
}

MaterialShader::~MaterialShader()
{
}


} // namespace renderer
} // namespace glrt
