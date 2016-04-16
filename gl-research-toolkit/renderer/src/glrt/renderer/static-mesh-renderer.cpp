#ifndef GLRT_RENDERER_STATICMESHRENDERER_INL
#define GLRT_RENDERER_STATICMESHRENDERER_INL

#include <glrt/renderer/static-mesh-renderer.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {
namespace implementation {

struct NoPrint
{
public:
  template<typename T>
  NoPrint operator<<(const T&){return NoPrint();}
};

#define LOG_MESH_LAODING NoPrint()
//#define LOG_MESH_LAODING qDebug() << "StaticMeshRecorder: "

StaticMeshRecorder::StaticMeshRecorder(gl::CommandListRecorder& recorder, ResourceManager& resourceManager, const Array<Uuid<Material>>& materialSet, TransformationBuffer& transformationBuffer, StaticMeshBufferManager& staticMeshBufferManager, const glm::ivec2& commonTokenList)
  : recorder(recorder),
    resourceManager(resourceManager),
    staticMeshBufferManager(staticMeshBufferManager),
    transformationBuffer(transformationBuffer),
    commonTokenList(commonTokenList)
{
  initMaterials(materialSet);

#if !GLRT_SUPPORT_UPDATE_MOVABLE_UNIFORMS_SEPERATELY
  bindNotMovableTokens();
#endif
}

void StaticMeshRecorder::bindNotMovableTokens()
{
  boundTokenRanges = &tokenRanges.tokenRangeNotMovable;
}

void StaticMeshRecorder::bindMovableTokens()
{
  boundTokenRanges = &tokenRanges.tokenRangeMovables;
}

void StaticMeshRecorder::unbindTokens()
{
  boundTokenRanges = nullptr;
}

void StaticMeshRecorder::bindMaterialType(Material::Type materialType)
{
  currentMaterialType = materialType;

  LOG_MESH_LAODING << "bindMaterialType(" << Material::typeToString(materialType) << ")";

  recorder.beginTokenListWithCopy(commonTokenList);
}

void StaticMeshRecorder::unbindMaterialType(Material::Type materialType)
{
  Q_ASSERT(boundTokenRanges != nullptr);
  boundTokenRanges->insert(materialType, recorder.endTokenList());
}

void StaticMeshRecorder::bindMaterial(const Uuid<Material>& material)
{
  Q_ASSERT(materialGpuAddresses.contains(material)); // if the material is not known, the mateiral wasn't initialized correctly

  if(currentMaterialType.testFlag(Material::TypeFlag::FRAGMENT_SHADER_UNIFORM))
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderObject::ShaderType::FRAGMENT, materialGpuAddresses.value(material));
  else if(currentMaterialType.testFlag(Material::TypeFlag::VERTEX_SHADER_UNIFORM))
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderObject::ShaderType::VERTEX, materialGpuAddresses.value(material));
  else
    Q_UNREACHABLE();

  LOG_MESH_LAODING << "bindMaterial(" << labelForUuid(material) << ")";
}

void StaticMeshRecorder::unbindMaterial(const Uuid<Material>& material)
{
  Q_UNUSED(material);
  LOG_MESH_LAODING;
}

void StaticMeshRecorder::bindMesh(const Uuid<StaticMesh>& mesh)
{
  currentMesh = mesh;

  LOG_MESH_LAODING << "bindMesh(" << labelForUuid(mesh) << ")";
}

void StaticMeshRecorder::unbindMesh(const Uuid<StaticMesh>& mesh)
{
  Q_ASSERT(currentMesh == mesh);
  currentMesh = Uuid<StaticMesh>();
}

void StaticMeshRecorder::drawInstances(int begin, int end)
{
  LOG_MESH_LAODING << "drawInstances(" << end-begin << ")";

  Q_ASSERT(currentMesh != Uuid<StaticMesh>());

  StaticMeshBuffer* staticMesh = staticMeshBufferManager.meshForUuid(currentMesh);

  staticMesh->recordBind(recorder);
  for(int i=begin; i<end; ++i)
  {
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, gl::ShaderObject::ShaderType::VERTEX, transformationBuffer.gpuAddressForInstance(i));
    staticMesh->recordDraw(recorder);
  }
}

void StaticMeshRecorder::initMaterials(const Array<Uuid<Material>>& materialSet)
{
  MaterialBuffer::Initializer materialBufferInitializer(recorder, resourceManager);

  materialBufferInitializer.begin(materialSet.length());
  for(Uuid<Material> m : materialSet)
    materialBufferInitializer.append(m);
  this->materialBuffer = std::move(materialBufferInitializer.end());

  materialGpuAddresses = materialBufferInitializer.gpuAddresses;
}


} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
