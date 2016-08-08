#ifndef GLRT_RENDERER_STATICMESHRENDERER_H
#define GLRT_RENDERER_STATICMESHRENDERER_H


#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/transformation-buffer.h>
#include <glrt/scene/scene-data.h>


namespace glrt {
namespace renderer {

struct TokenRanges
{
  QMap<Material::Type, glm::ivec2> tokenRange;
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

  void bindTokens();
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


class StaticMeshRenderer final
{
public:
  typedef typename glrt::scene::resources::Material Material;

  StaticMeshRenderer(scene::Scene& scene, StaticMeshBufferManager* staticMeshBufferManager);

  void update();

  bool needRerecording() const;
  TokenRanges recordCommandList(gl::CommandListRecorder& recorder, const glm::ivec2& commonTokenList);

private:
  typedef scene::Scene::Data::StaticMeshes StaticMeshes;

  scene::Scene::Data& scene_data;
  StaticMeshes*& staticMeshes;
  MaterialBuffer materialBuffer;
  TransformationBuffer transformationBuffer;
  StaticMeshBufferManager& staticMeshBufferManager;

  void updateObjectUniforms();
  void updateObjectUniforms(quint16 begin, quint16 end);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_STATICMESHRENDERER_H
