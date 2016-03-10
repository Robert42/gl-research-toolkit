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
QMap<Material::Type, glm::ivec2> StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::recordCommandList(gl::CommandListRecorder& recorder, const glm::ivec2& commonTokenList)
{
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  meshComponents.dirty = false;
  fragmentedArray.updateSegments(nullptr);

  const int length = fragmentedArray.length();

  if(length == 0)
  {
    transformationBuffer = std::move(TransformationBuffer());
    materialBuffer = std::move(MaterialBuffer());
    return QMap<Material::Type, glm::ivec2>();
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

  return staticMeshRecorder.tokenRanges;
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::updateMovableObjectUniforms()
{
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;


#if GLRT_SUPPORT_UPDATE_MOVABLE_UNIFORMS_SEPERATELY
  glm::ivec2 range = fragmentedArray.section_boundaries(glrt::scene::Node::Component::MovabilityHint::MOVABLE);

  updateObjectUniforms(range.x, range.y);
#else
  updateObjectUniforms(0, fragmentedArray.length());
#endif
}

template<class T_Component, class T_Recorder, typename T_FragmentedArray, typename T_BufferCapacityTraits>
void StaticMeshRenderer<T_Component, T_Recorder, T_FragmentedArray, T_BufferCapacityTraits>::updateObjectUniforms(int begin, int end)
{
  FragmentedArray& fragmentedArray = meshComponents.fragmented_array;

  transformationBuffer.update(begin, end, const_cast<const T_Component**>(fragmentedArray.data()), fragmentedArray.length());
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
