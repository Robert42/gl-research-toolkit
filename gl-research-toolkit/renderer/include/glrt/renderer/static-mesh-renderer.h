#ifndef GLRT_RENDERER_STATICMESHRENDERER_H
#define GLRT_RENDERER_STATICMESHRENDERER_H


#include <glrt/renderer/material-buffer.h>
#include <glrt/renderer/transformation-buffer.h>
#include <glrt/scene/scene-data.h>


namespace glrt {
namespace renderer {

typedef QMap<Material::Type, glm::ivec2> TokenRanges;

namespace implementation {


struct StaticMeshRecorder final
{
public:
  gl::CommandListRecorder& recorder;
  const glm::ivec2 commonTokenList;
  TokenRanges tokenRanges;

  StaticMeshRecorder(gl::CommandListRecorder& recorder,
                     const glm::ivec2& commonTokenList);

  void bindMaterialType(Material::Type materialType);
  void unbindMaterialType();

  void bindMaterial(GLuint64 uniformBufer);
  void unbindMaterial();

  void bindMesh(StaticMeshBuffer* staticMesh);
  void unbindMesh();

  void appendDraw(GLuint64 transformUniform);

private:
  StaticMeshBuffer* currentStaticMesh = nullptr;
  Material::Type currentMaterialType;
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
