#ifndef GLRT_RENDERER_SAMPLERESOURCEMANAGER_H
#define GLRT_RENDERER_SAMPLERESOURCEMANAGER_H

#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/resource-index.h>
#include <glrt/renderer/static-mesh-buffer-manager.h>

namespace glrt {
namespace renderer {

class SampleResourceManager final : public scene::resources::ResourceManager
{
public:
  Index index;
  StaticMeshBufferManager* staticMeshBufferManager;

  static Uuid<scene::Scene> cornellBoxScene();
  static Uuid<scene::Scene> cornellBoxRoughnessScene();

  SampleResourceManager();
  ~SampleResourceManager();

protected:
  bool foreachIndexImpl(const std::function<bool(const Index*)>& lambda) const final override;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SAMPLERESOURCEMANAGER_H
