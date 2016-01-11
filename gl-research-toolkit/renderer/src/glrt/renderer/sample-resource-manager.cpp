#include <glrt/renderer/sample-source-manager.h>

namespace glrt {
namespace renderer {


SampleResourceManager::SampleResourceManager()
  : ResourceManager(nullptr), // #TODO: use a real static mesh loader
    index(glrt::Uuid<glrt::scene::resources::ResourceIndex>("{cf685c44-8d67-4531-b4f2-964acef0ec10}"))
{
  index.loadIndexedDirectory(GLRT_ASSET_DIR);
}

SampleResourceManager::~SampleResourceManager()
{
}

SampleResourceManager::Index* SampleResourceManager::indexForResourceUuid(const QUuid& uuid, Index* fallback)
{
  if(index.isRegistered(uuid))
    return &index;
  else
    return fallback;
}


} // namespace renderer
} // namespace glrt

