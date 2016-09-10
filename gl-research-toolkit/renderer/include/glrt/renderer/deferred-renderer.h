#ifndef GLRT_RENDERER_DEFERREDRENDERER_H
#define GLRT_RENDERER_DEFERREDRENDERER_H

#include "scene-renderer.h"
#include <glrt/renderer/sample-resource-manager.h>
#include <glrt/renderer/forward-renderer.h>

namespace glrt {
namespace renderer {

class DeferredRenderer : public ForwardRenderer
{
public:
  DeferredRenderer(const glm::ivec2& videoResolution, scene::Scene* scene, SampleResourceManager* resourceManager, debugging::ShaderDebugPrinter* debugPrinter);
  ~DeferredRenderer();
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEFERREDRENDERER_H
