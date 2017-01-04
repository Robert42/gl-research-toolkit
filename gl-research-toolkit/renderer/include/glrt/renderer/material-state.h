#ifndef GLRT_RENDERER_MATERIALSHADER_H
#define GLRT_RENDERER_MATERIALSHADER_H

#include <glrt/renderer/gl/command-list-recorder.h>

namespace glrt {
namespace renderer {



const float maskAlphaThreshold = 0.5f;

class MaterialState
{
  Q_DISABLE_COPY(MaterialState)
public:
  enum class Flags : quint32
  {
    NONE = 0x0,
    DEPTH_TEST = 0x1,
    DEPTH_WRITE = 0x2,
    COLOR_WRITE = 0x4,
    ALPHA_WRITE = 0x8,
    ALPHA_BLENDING = 0x10,
    NO_FACE_CULLING = 0x20,
    NO_STATIC_MESH = 0x40,
  };

  friend Flags operator|(Flags a, Flags b){return Flags(quint32(a) | quint32(b));}
  friend Flags operator&(Flags a, Flags b){return Flags(quint32(a) & quint32(b));}

  gl::FramebufferObject* framebuffer = nullptr;
  gl::StatusCapture stateCapture;
  int shader;

  MaterialState(int shader, Flags flags);
  MaterialState(MaterialState&& other);
  MaterialState& operator=(MaterialState&& other);

  ~MaterialState();

  void activateStateForFlags();
  void deactivateStateForFlags();

private:
  Flags flags;

  bool hasFlag(Flags flag) const;
  GLboolean glHasFlag(Flags flag) const;
};

} // namespace renderer


template<>
struct DefaultTraits<renderer::MaterialState>
{
  typedef ArrayTraits_mCmOD<renderer::MaterialState> type;
};

} // namespace glrt

#endif // GLRT_RENDERER_MATERIALSHADER_H
