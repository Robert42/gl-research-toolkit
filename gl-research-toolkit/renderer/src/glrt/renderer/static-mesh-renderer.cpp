#include <glrt/renderer/static-mesh-renderer.h>
#include <glrt/glsl/layout-constants.h>
#include <glrt/renderer/gl/shader-type.h>
#include <glrt/toolkit/profiler.h>

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



StaticMeshRecorder::StaticMeshRecorder(gl::CommandListRecorder& recorder,
                                       const glm::ivec2& commonTokenList)
  : recorder(recorder),
    commonTokenList(commonTokenList)
{
}

void StaticMeshRecorder::bindMaterialType(Material::Type materialType)
{
  currentMaterialType = materialType;

  LOG_MESH_LAODING << "bindMaterialType(" << Material::typeToString(materialType) << ")";

  recorder.beginTokenListWithCopy(commonTokenList);
}

void StaticMeshRecorder::unbindMaterialType()
{
  tokenRanges.insert(currentMaterialType, recorder.endTokenList());
}

void StaticMeshRecorder::bindMaterial(GLuint64 uniformBufer)
{
  if(currentMaterialType.testFlag(Material::TypeFlag::FRAGMENT_SHADER_UNIFORM))
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderType::FRAGMENT, uniformBufer);
  else if(currentMaterialType.testFlag(Material::TypeFlag::VERTEX_SHADER_UNIFORM))
    recorder.append_token_UniformAddress(UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, gl::ShaderType::VERTEX, uniformBufer);
  else
    Q_UNREACHABLE();

  LOG_MESH_LAODING << "bindMaterial()";
}

void StaticMeshRecorder::unbindMaterial()
{
}

void StaticMeshRecorder::bindMesh(StaticMeshBuffer* staticMesh)
{
  currentStaticMesh = staticMesh;

  currentStaticMesh->recordBind(recorder);
}

void StaticMeshRecorder::unbindMesh()
{
  currentStaticMesh = nullptr;
}

void StaticMeshRecorder::appendDraw(GLuint64 transformUniform)
{
  Q_ASSERT(currentStaticMesh!=nullptr);

  recorder.append_token_UniformAddress(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, gl::ShaderType::VERTEX, transformUniform);
  currentStaticMesh->recordDraw(recorder);
}


} // namespace implementation

// ======== StaticMeshRenderer =================================================

StaticMeshRenderer::StaticMeshRenderer(scene::Scene& scene, StaticMeshBufferManager* staticMeshBufferManager)
  : scene_data(*scene.data),
    staticMeshes(scene_data.staticMeshes),
    materialBuffer(scene_data.resourceManager, scene_data.staticMeshes->capacity()),
    transformationBuffer(scene_data.staticMeshes->capacity()),
    staticMeshBufferManager(*staticMeshBufferManager)
{
}

void StaticMeshRenderer::update()
{
  PROFILE_SCOPE("StaticMeshRenderer::update()")
  updateObjectUniforms();
}

bool StaticMeshRenderer::needRerecording() const
{
  return staticMeshes->dirtyOrder;
}

TokenRanges StaticMeshRenderer::recordCommandList(gl::CommandListRecorder& recorder, const glm::ivec2& commonTokenList)
{
  PROFILE_SCOPE("StaticMeshRenderer::recordCommandList()")
  scene_data.sort_staticMeshes();

  const quint16 length = staticMeshes->length;

  if(Q_UNLIKELY(length == 0))
    return TokenRanges();

  const Uuid<Material>* materials = staticMeshes->materialUuid;
  const Uuid<StaticMesh>* meshes = staticMeshes->staticMeshUuid;
  Uuid<Material> prevMaterial;
  Uuid<StaticMesh> prevMesh;
  Material::Type prevMaterialType = Material::TypeFlag(0xffffffff);
  const GLuint64 materialUniformBuffer = materialBuffer.buffer.gpuBufferAddress();
  GLuint64 currentMaterialUniformBuffer = materialUniformBuffer;

  implementation::StaticMeshRecorder staticMeshRecorder(recorder,
                                                        commonTokenList);

  Q_ASSERT(materials[0] != prevMaterial);
  Q_ASSERT(meshes[0] != prevMesh);

  materialBuffer.map(length);
  for(quint16 i=0; i<length; ++i)
  {
    if(prevMaterial != materials[i])
    {
      prevMaterial = materials[i];
      Material::Type materialType = materialBuffer.append(materials[i]);

      if(Q_LIKELY(i!=0))
        staticMeshRecorder.unbindMaterial();
      if(Q_UNLIKELY(materialType != prevMaterialType))
      {
        if(Q_LIKELY(i!=0))
          staticMeshRecorder.unbindMaterialType();
        prevMaterialType = materialType;
        staticMeshRecorder.bindMaterialType(materialType);
      }
      staticMeshRecorder.bindMaterial(currentMaterialUniformBuffer);
      currentMaterialUniformBuffer += materialBuffer.blockOffset;
    }

    if(prevMesh != meshes[i])
    {
      if(Q_LIKELY(i!=0))
        staticMeshRecorder.unbindMesh();

      prevMesh = meshes[i];
      staticMeshRecorder.bindMesh(staticMeshBufferManager.meshForUuid(meshes[i]));
    }

    staticMeshRecorder.appendDraw(transformationBuffer.gpuAddressForInstance(i));
  }
  staticMeshRecorder.unbindMesh();
  staticMeshRecorder.unbindMaterial();
  staticMeshRecorder.unbindMaterialType();
  materialBuffer.unmap();

  Q_ASSERT(materialUniformBuffer == materialBuffer.buffer.gpuBufferAddress());

  updateObjectUniforms(0, length);

  return staticMeshRecorder.tokenRanges;
}

void StaticMeshRenderer::updateObjectUniforms()
{
  if(Q_UNLIKELY(staticMeshes->dirtyOrder || staticMeshes->numDynamic>0))
    updateObjectUniforms(0, staticMeshes->length);
}

void StaticMeshRenderer::updateObjectUniforms(quint16 begin, quint16 end)
{
  transformationBuffer.update(begin, end, scene_data.transformDataForClass(scene::Node::Component::DataClass::STATICMESH));
}

} // namespace renderer
} // namespace glrt
