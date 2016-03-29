#include <glrt/renderer/sample-resource-manager.h>

namespace glrt {
namespace renderer {


SampleResourceManager::SampleResourceManager()
  : ResourceManager(staticMeshBufferManager = new StaticMeshBufferManager(this), glTextureManager = new GlTextureManager(this)),
    index(glrt::Uuid<glrt::scene::resources::ResourceIndex>("{cf685c44-8d67-4531-b4f2-964acef0ec10}"))
{
  index.loadIndexedDirectory(GLRT_ASSET_DIR);
}

SampleResourceManager::~SampleResourceManager()
{
}

bool SampleResourceManager::foreachIndexImpl(const std::function<bool(const Index* index)>& lambda) const
{
  return lambda(&index);
}


Uuid<scene::Scene> SampleResourceManager::cornellBoxScene()
{
  return Uuid<scene::Scene>("{158da3d3-ef79-4895-add4-f2fe22c0dbff}");
}

Uuid<scene::Scene> SampleResourceManager::cornellBoxRoughnessScene()
{
  return Uuid<scene::Scene>("{297e2f65-220b-42d7-ae10-1b9b45a75f7c}");
}

Uuid<scene::Scene> SampleResourceManager::cornellBoxSuzanneScene()
{
  return Uuid<scene::Scene>("{e8a07aee-d0d7-43b7-a9e2-867a824d2f48}");
}

Uuid<scene::resources::StaticMesh> SampleResourceManager::suzanneLowPoly()
{
  return Uuid<scene::resources::StaticMesh>("{0ecdf00b-f8fe-4988-aa4a-cda5c2644c83}");
}

Uuid<scene::resources::StaticMesh> SampleResourceManager::suzanneSmooth()
{
  return Uuid<scene::resources::StaticMesh>("{7eed8ff5-ba25-424d-aa2a-7a737ee3ce4d}");
}


} // namespace renderer
} // namespace glrt

