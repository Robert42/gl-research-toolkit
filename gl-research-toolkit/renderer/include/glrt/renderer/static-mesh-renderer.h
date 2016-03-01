#ifndef GLRT_RENDERER_STATICMESHRENDERER_H
#define GLRT_RENDERER_STATICMESHRENDERER_H

#include <glrt/renderer/implementation/fragmented-static-mesh-component-array.h>
#include <glrt/renderer/synced-fragmented-component-array.h>
#include <glrt/renderer/material-buffer.h>


namespace glrt {
namespace renderer {
namespace implementation {

struct StaticMeshRecorder final
{
public:
  typedef typename glrt::scene::resources::Material Material;
  typedef typename glrt::scene::resources::StaticMesh StaticMesh;
  typedef typename glrt::scene::resources::ResourceManager ResourceManager;

  gl::CommandListRecorder& recorder;
  ResourceManager& resourceManager;

  MaterialBuffer::Initializer materialBuffer;

  StaticMeshRecorder(gl::CommandListRecorder& recorder, ResourceManager& resourceManager, StaticMeshBufferManager& staticMeshBufferManager);

  void bindMaterial(const Uuid<Material>& material);
  void unbindMaterial(const Uuid<Material>& material);

  void bindMesh(const Uuid<StaticMesh>& mesh);
  void unbindMesh(const Uuid<StaticMesh>& mesh);

  void drawInstances(int num);

private:
  Uuid<StaticMesh> currentMesh;
  StaticMeshBufferManager& staticMeshBufferManager;
};

} // namespace implementation


template<class T_Component=scene::StaticMeshComponent, class T_Recorder=implementation::StaticMeshRecorder, typename T_FragmentedArray=typename implementation::FragmentedStaticMeshComponentArray<T_Component, T_Recorder>::type, typename T_BufferCapacityTraits=ArrayCapacityTraits_Capacity_Blocks<512, 4096>>
class StaticMeshRenderer final
{
public:
  StaticMeshRenderer(scene::Scene& scene, StaticMeshBufferManager* staticMeshBufferManager);

  void update();

  bool needRerecording() const;
  void recordCommandList(gl::CommandListRecorder& recorder);

private:
  typedef T_FragmentedArray FragmentedArray;
  typedef SyncedFragmentedComponentArray<T_Component, FragmentedArray> StaticMeshComponentArray;

  StaticMeshComponentArray meshComponents;
  MaterialBuffer materialBuffer;
  gl::Buffer objectUniforms;
  StaticMeshBufferManager& staticMeshBufferManager;

  void updateMovableObjectUniforms();
  void updateObjectUniforms(int begin, int end);
};

} // namespace renderer
} // namespace glrt

#include "static-mesh-renderer.inl"

#endif // GLRT_RENDERER_STATICMESHRENDERER_H
