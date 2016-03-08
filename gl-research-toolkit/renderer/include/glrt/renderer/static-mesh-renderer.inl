#ifndef GLRT_RENDERER_STATICMESHRENDERER_INL
#define GLRT_RENDERER_STATICMESHRENDERER_INL

#include "static-mesh-renderer.h"

#include <glrt/scene/node.h>
#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace renderer {


template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::StaticMeshRenderer(scene::Scene& scene, StaticMeshBufferManager* staticMeshBufferManager)
  : meshComponents(scene),
    staticMeshBufferManager(*staticMeshBufferManager)
{
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::update()
{
  updateMovableObjectUniforms();
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
bool StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::needRerecording() const
{
  return meshComponents.dirty;
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::recordCommandList(gl::CommandListRecorder& recorder)
{
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  meshComponents.dirty = false;
  fragmentedArray.updateSegments(nullptr);

  const int length = fragmentedArray.length();

  if(length == 0)
  {
    transformations = std::move(gl::Buffer());
    staticMeshInstancesUniform = std::move(gl::Buffer());
    return;
  }

  QSet<Uuid<Material>> materialSet;
  T_Component** components = fragmentedArray.data();
  glrt::scene::resources::ResourceManager& resourceManager = components[0]->resourceManager();

  transformations = std::move(gl::Buffer(sizeof(glm::mat4) * fragmentedArray.length(), gl::Buffer::MAP_WRITE));
  StaticMeshInstancesUniformBlock instancesBlock;
  instancesBlock.transformAddress = transformations.gpuBufferAddress();
  staticMeshInstancesUniform = std::move(gl::Buffer(sizeof(StaticMeshInstancesUniformBlock), gl::Buffer::IMMUTABLE, &instancesBlock));

  recorder.append_token_UniformAddress(UNIFORM_BINDING_MESH_INSTANCE_BLOCK, gl::ShaderObject::ShaderType::VERTEX, staticMeshInstancesUniform.gpuBufferAddress());

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
                                staticMeshBufferManager);

  fragmentedArray.iterate(&staticMeshRecorder);

  updateObjectUniforms(0, fragmentedArray.length());

  this->materialBuffer = std::move(staticMeshRecorder.materialBuffer);
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::updateMovableObjectUniforms()
{
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  glm::ivec2 range = fragmentedArray.section_boundaries(glrt::scene::Node::Component::MovabilityHint::MOVABLE);

  updateObjectUniforms(range.x, range.y);
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::updateObjectUniforms(int begin, int end)
{
  const int length = end-begin;
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  const int n = fragmentedArray.length();
  T_Component** component = fragmentedArray.data();
  glm::mat4* transformation  =reinterpret_cast<glm::mat4*>(transformations.Map(begin * sizeof(glm::mat4), length * sizeof(glm::mat4), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));

  Q_ASSERT(begin<=end);
  Q_ASSERT(end<=n);


  for(int i=0; i<length; ++i)
    transformation[i] = component[i]->globalCoordFrame().toMat4();

  transformations.Unmap();
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
