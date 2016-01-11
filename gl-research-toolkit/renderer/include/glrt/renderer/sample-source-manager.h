#ifndef GLRT_RENDERER_SAMPLERESOURCEMANAGER_H
#define GLRT_RENDERER_SAMPLERESOURCEMANAGER_H

#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/resource-index.h>

namespace glrt {
namespace renderer {

class SampleResourceManager final : public scene::resources::ResourceManager
{
public:
  glrt::scene::resources::ResourceIndex index;

  SampleResourceManager();
  ~SampleResourceManager();

  Index* indexForResourceUuid(const QUuid& uuid, Index* fallback=nullptr) final override;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SAMPLERESOURCEMANAGER_H
