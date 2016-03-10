#ifndef GLRT_RENDERER_MATERIALSHADER_H
#define GLRT_RENDERER_MATERIALSHADER_H

#include <glrt/renderer/gl/command-list-recorder.h>

namespace glrt {
namespace renderer {

class MaterialState
{
  Q_DISABLE_COPY(MaterialState)
public:
  gl::FramebufferObject* framebuffer = nullptr;
  gl::StatusCapture stateCapture;
  int shader;

  MaterialState(int shader);
  MaterialState(MaterialState&& other);
  MaterialState& operator=(MaterialState&& other);

  ~MaterialState();
};

} // namespace renderer


template<>
struct DefaultTraits<renderer::MaterialState>
{
  typedef ArrayTraits_mCmOD<renderer::MaterialState> type;
};

} // namespace glrt

#endif // GLRT_RENDERER_MATERIALSHADER_H
