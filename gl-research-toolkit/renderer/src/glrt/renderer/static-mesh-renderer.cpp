#include <glrt/renderer/static-mesh-renderer.h>
#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/gl/shader-type.h>

namespace glrt {
namespace renderer {
namespace implementation {



struct NoPrint
{
  template<typename T>
  NoPrint operator<<(const T&){return NoPrint();}
};

#define LOG_MESH_LAODING NoPrint()
//#define LOG_MESH_LAODING qDebug() << "StaticMeshRecorder: "

// TODO:::::::::::::::::::::::::::
#if 0
StaticMeshRecorder::StaticMeshRecorder(gl::CommandListRecorder& recorder, ResourceManager& resourceManager, const Array<Uuid<Material>>& materialSet, TransformationBuffer& transformationBuffer, StaticMeshBufferManager& staticMeshBufferManager, const glm::ivec2& commonTokenList)
  : recorder(recorder),
    resourceManager(resourceManager),
    staticMeshBufferManager(staticMeshBufferManager),
    transformationBuffer(transformationBuffer),
    commonTokenList(commonTokenList)
{
  initMaterials(materialSet);

  bindTokens();
}

void StaticMeshRecorder::bindTokens()
{
  boundTokenRanges = &tokenRanges.tokenRange;
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
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderType::FRAGMENT, materialGpuAddresses.value(material));
  else if(currentMaterialType.testFlag(Material::TypeFlag::VERTEX_SHADER_UNIFORM))
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderType::VERTEX, materialGpuAddresses.value(material));
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
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, gl::ShaderType::VERTEX, transformationBuffer.gpuAddressForInstance(i));
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

#endif

} // namespace implementation

// ======== StaticMeshRenderer =================================================

StaticMeshRenderer::StaticMeshRenderer(scene::Scene& scene, StaticMeshBufferManager* staticMeshBufferManager)
  : scene_data(*scene.data),
    staticMeshes(scene_data.staticMeshes),
    staticMeshBufferManager(*staticMeshBufferManager)
{
}

void StaticMeshRenderer::update()
{
  updateObjectUniforms();
}

bool StaticMeshRenderer::needRerecording() const
{
  return staticMeshes->dirtyOrder;
}

TokenRanges StaticMeshRenderer::recordCommandList(gl::CommandListRecorder& recorder, const glm::ivec2& commonTokenList)
{
  scene_data.sort_staticMeshes();

// TODO:::::::::::::::::::::::::::
#if 0
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  meshComponents.dirty = false;
  fragmentedArray.updateSegments(nullptr);

  const int length = fragmentedArray.length();

  if(length == 0)
  {
    transformationBuffer = std::move(TransformationBuffer());
    materialBuffer = std::move(MaterialBuffer());
    return TokenRanges();
  }

  QSet<Uuid<Material>> materialSet;
  T_Component** components = fragmentedArray.data();
  glrt::scene::resources::ResourceManager& resourceManager = components[0]->resourceManager();

  transformationBuffer.init(const_cast<const T_Component**>(fragmentedArray.data()), fragmentedArray.length());

  for(int i=0; i<length; ++i)
    materialSet.insert(components[i]->materialUuid);
  Array<Uuid<Material>> allMaterials;
  allMaterials.reserve(materialSet.size());
  for(Uuid<Material> m : materialSet)
    allMaterials.append(m);
  allMaterials.sort([&resourceManager](Uuid<Material> a, Uuid<Material> b){return implementation::materialLessThan(resourceManager.materialForUuid(a), resourceManager.materialForUuid(b), a, b);});

  T_Recorder staticMeshRecorder(recorder,
                                resourceManager,
                                allMaterials,
                                transformationBuffer,
                                staticMeshBufferManager,
                                commonTokenList);

  fragmentedArray.iterate(&staticMeshRecorder);

  updateObjectUniforms(0, fragmentedArray.length());

  this->materialBuffer = std::move(staticMeshRecorder.materialBuffer);

  // TODO: call in  the right place!! staticMeshes.dirtyOrder = false;

  return staticMeshRecorder.tokenRanges;
#endif
  return TokenRanges();
}

void StaticMeshRenderer::updateObjectUniforms()
{
  updateObjectUniforms(0, staticMeshes->length);
}

void StaticMeshRenderer::updateObjectUniforms(quint16 begin, quint16 end)
{
// TODO:::::::::::::::::::::::::::
#if 0
  transformationBuffer.update(begin, end, const_cast<const T_Component**>(fragmentedArray.data()), fragmentedArray.length());
#endif
}

} // namespace renderer
} // namespace glrt
