#include <glrt/renderer/debugging/debugging-posteffect.h>

namespace glrt {
namespace renderer {
namespace debugging {


DebuggingPosteffect::DebuggingPosteffect(scene::Scene* scene, ReloadableShader&& shader, bool depthTest)
  : DebugRenderer(scene),
    _shader(std::move(shader)),
    _depthTest(depthTest)
{
}

DebuggingPosteffect::DebuggingPosteffect(DebuggingPosteffect&& other)
  : DebugRenderer(std::move(other)),
    _shader(std::move(other._shader)),
    _depthTest(other._depthTest)
{
  other._scene = nullptr;
  other._depthTest = false;
}

DebuggingPosteffect::~DebuggingPosteffect()
{
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
