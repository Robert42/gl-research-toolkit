#include <glrt/renderer/deferred-renderer.h>

namespace glrt {
namespace renderer {

DeferredRenderer::DeferredRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter)
  : ForwardRenderer(videoResolution, scene, resourceManager, debugPrinter)
{
}

DeferredRenderer::~DeferredRenderer()
{
}

} // namespace renderer
} // namespace glrt
