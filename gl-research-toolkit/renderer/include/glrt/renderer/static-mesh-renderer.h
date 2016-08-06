#ifndef GLRT_RENDERER_STATICMESHRENDERER_H
#define GLRT_RENDERER_STATICMESHRENDERER_H

#if 0

#include <glrt/renderer/synced-fragmented-component-array.h>
#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/transformation-buffer.h>


namespace glrt {
namespace renderer {

struct TokenRanges
{
  QMap<Material::Type, glm::ivec2> tokenRangeDynamics;
  QMap<Material::Type, glm::ivec2> tokenRangeNotDynamic;
};

namespace implementation {


struct StaticMeshRecorder final
{
public:
  typedef typename glrt::scene::resources::Material Material;
  typedef typename glrt::scene::resources::StaticMesh StaticMesh;
  typedef typename glrt::scene::resources::ResourceManager ResourceManager;

  gl::CommandListRecorder& recorder;
  ResourceManager& resourceManager;

  MaterialBuffer materialBuffer;

  TokenRanges tokenRanges;

  StaticMeshRecorder(gl::CommandListRecorder& recorder, ResourceManager& resourceManager, const Array<Uuid<Material>>& materialSet, TransformationBuffer& transformationBuffer, StaticMeshBufferManager& staticMeshBufferManager, const glm::ivec2& commonTokenList);

  void bindNotDynamicTokens();
  void bindDynamicTokens();
  void unbindTokens();

  void bindMaterialType(Material::Type materialType);
  void unbindMaterialType(Material::Type materialType);

  void bindMaterial(const Uuid<Material>& material);
  void unbindMaterial(const Uuid<Material>& material);

  void bindMesh(const Uuid<StaticMesh>& mesh);
  void unbindMesh(const Uuid<StaticMesh>& mesh);

  void drawInstances(int begin, int end);

private:
  QMap<Uuid<Material>, GLuint64> materialGpuAddresses;
  Uuid<StaticMesh> currentMesh;
  StaticMeshBufferManager& staticMeshBufferManager;
  TransformationBuffer& transformationBuffer;

  const glm::ivec2 commonTokenList;
  QMap<Material::Type, glm::ivec2>* boundTokenRanges = nullptr;

  Material::Type currentMaterialType;

  void initMaterials(const Array<Uuid<Material>>& materialSet);
};

} // namespace implementation


template<class T_Component=scene::StaticMeshComponent, class T_Recorder=implementation::StaticMeshRecorder, typename T_BufferCapacityTraits=ArrayCapacityTraits_Capacity_Blocks<512, 4096>>
class StaticMeshRenderer final
{
public:
  typedef typename glrt::scene::resources::Material Material;

  StaticMeshRenderer(scene::Scene& scene, StaticMeshBufferManager* staticMeshBufferManager);

  void update();

  bool needRerecording() const;
  TokenRanges recordCommandList(gl::CommandListRecorder& recorder, const glm::ivec2& commonTokenList);

private:
  MaterialBuffer materialBuffer;
  TransformationBuffer transformationBuffer;
  StaticMeshBufferManager& staticMeshBufferManager;

  void updateDynamicObjectUniforms();
  void updateObjectUniforms(int begin, int end);
};

} // namespace renderer
} // namespace glrt

#include "static-mesh-renderer.inl"

#endif
#endif // GLRT_RENDERER_STATICMESHRENDERER_H
