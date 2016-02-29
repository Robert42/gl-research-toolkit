#ifndef GLRT_RENDERER_STATICMESHRENDERER_INL
#define GLRT_RENDERER_STATICMESHRENDERER_INL

#include <glrt/renderer/static-mesh-renderer.h>

namespace glrt {
namespace renderer {
namespace implementation {


StaticMeshRecorder::StaticMeshRecorder(gl::CommandListRecorder& recorder)
  : recorder(recorder)
{
}

void StaticMeshRecorder::bindMaterial(const Uuid<Material>& material)
{
}

void StaticMeshRecorder::unbindMaterial(const Uuid<Material>& material)
{
}

void StaticMeshRecorder::bindMesh(const Uuid<StaticMesh>& mesh)
{
}

void StaticMeshRecorder::unbindMesh(const Uuid<StaticMesh>& mesh)
{
}

void StaticMeshRecorder::drawInstances(int num)
{
}


} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
