#ifndef GLRT_RENDERER_STATICMESHRENDERER_INL
#define GLRT_RENDERER_STATICMESHRENDERER_INL

#include <glrt/renderer/static-mesh-renderer.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {
namespace implementation {


StaticMeshRecorder::StaticMeshRecorder(gl::CommandListRecorder& recorder, ResourceManager& resourceManager, StaticMeshBufferManager& staticMeshBufferManager)
  : recorder(recorder),
    resourceManager(resourceManager),
    materialBuffer(recorder, resourceManager),
    staticMeshBufferManager(staticMeshBufferManager)
{
  // #TODO fill the material Buffer here?
}

void StaticMeshRecorder::bindMaterial(const Uuid<Material>& material)
{
  Q_ASSERT(materialBuffer.gpuAddresses.contains(material)); // if the material is not known, the mateiral wasn't initialized correctly

  recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderObject::ShaderType::FRAGMENT, materialBuffer.gpuAddresses.value(material));
}

void StaticMeshRecorder::unbindMaterial(const Uuid<Material>& material)
{
  Q_UNUSED(material);
}

void StaticMeshRecorder::bindMesh(const Uuid<StaticMesh>& mesh)
{
  currentMesh = mesh;
}

void StaticMeshRecorder::unbindMesh(const Uuid<StaticMesh>& mesh)
{
  Q_ASSERT(currentMesh == mesh);
  currentMesh = Uuid<StaticMesh>();
}

void StaticMeshRecorder::drawInstances(int num)
{
  Q_ASSERT(currentMesh != Uuid<StaticMesh>());

  StaticMeshBuffer* staticMesh = staticMeshBufferManager.meshForUuid(currentMesh);

  staticMesh->recordDraw(recorder, num);
}


} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
