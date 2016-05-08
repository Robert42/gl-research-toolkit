#ifndef GLRT_RENDERER_MANAGEDSHADER_H
#define GLRT_RENDERER_MANAGEDSHADER_H

#include <glrt/renderer/dependencies.h>

#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {

class ManagedShader
{
public:
  ManagedShader();
  ~ManagedShader();

  ManagedShader(ManagedShader&& other);
  void operator=(ManagedShader&& other);

  ManagedShader(const ManagedShader&) = delete;
  void operator=(const ManagedShader&) = delete;

private:
  GLuint _shaderObject;
};

} // namespace renderer

template<>
struct DefaultTraits<glrt::renderer::ManagedShader>
{
  typedef ArrayTraits_mCmOD<glrt::renderer::ManagedShader> type;
};

} // namespace glrt

#endif // GLRT_RENDERER_MANAGEDSHADER_H
