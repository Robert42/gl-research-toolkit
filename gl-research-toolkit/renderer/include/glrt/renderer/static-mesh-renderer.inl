#ifndef GLRT_RENDERER_STATICMESHRENDERER_INL
#define GLRT_RENDERER_STATICMESHRENDERER_INL

#include "static-mesh-renderer.h"

#include <glrt/scene/node.h>

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

  if(fragmentedArray.length() == 0)
  {
    objectUniforms = std::move(gl::Buffer());
    return;
  }

  T_Recorder staticMeshRecorder(recorder,
                                fragmentedArray.data()[0]->resourceManager(),
                                staticMeshBufferManager);

  fragmentedArray.iterate(&staticMeshRecorder);

  objectUniforms = std::move(gl::Buffer(sizeof(glm::mat4) * fragmentedArray.length(), gl::Buffer::MAP_WRITE));
  updateObjectUniforms(0, fragmentedArray.length());
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
  // #TODO
}


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_INL
