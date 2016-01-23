#include <glrt/renderer/sample-resource-manager.h>

namespace glrt {
namespace renderer {


SampleResourceManager::SampleResourceManager()
  : ResourceManager(staticMeshBufferManager = new StaticMeshBufferManager),
    index(glrt::Uuid<glrt::scene::resources::ResourceIndex>("{cf685c44-8d67-4531-b4f2-964acef0ec10}"))
{
  index.loadIndexedDirectory(GLRT_ASSET_DIR);
}

SampleResourceManager::~SampleResourceManager()
{
}

void SampleResourceManager::foreachIndexImpl(const std::function<bool(const Index* index)>& lambda) const
{
  lambda(&index);
}


Uuid<scene::Scene> SampleResourceManager::cornellBoxScene()
{
  return Uuid<scene::Scene>("{158da3d3-ef79-4895-add4-f2fe22c0dbff}");
}


Uuid<scene::Scene> SampleResourceManager::cornellBoxRoughnessScene()
{
  return Uuid<scene::Scene>("{297e2f65-220b-42d7-ae10-1b9b45a75f7c}");
}


} // namespace renderer
} // namespace glrt

