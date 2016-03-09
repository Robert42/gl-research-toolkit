#ifndef GLRT_RENDERER_MATERIALSHADER_H
#define GLRT_RENDERER_MATERIALSHADER_H

#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/gl/command-list-recorder.h>

namespace glrt {
namespace renderer {

class MaterialShader
{
  Q_DISABLE_COPY(MaterialShader)
public:
  gl::FramebufferObject* framebuffer = nullptr;
  gl::StatusCapture stateCapture;
  ReloadableShader shader;

  MaterialShader(ReloadableShader&& shader);
  MaterialShader(const QSet<QString>& preprocessorBlock);
  MaterialShader(MaterialShader&& other);
  MaterialShader& operator=(MaterialShader&& other);

  ~MaterialShader();
};

} // namespace renderer


template<>
struct DefaultTraits<renderer::MaterialShader>
{
  typedef ArrayTraits_mCmOD<renderer::MaterialShader> type;
};

} // namespace glrt

#endif // GLRT_RENDERER_MATERIALSHADER_H
