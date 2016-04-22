#ifndef GLRT_RENDERER_VOXELIZEDSCENE_H
#define GLRT_RENDERER_VOXELIZEDSCENE_H

#include <glrt/scene/scene.h>

namespace glrt {
namespace renderer {

class VoxelizedScene : public QObject
{
  Q_OBJECT
public:
  glrt::scene::Scene& scene;

  // The given instance must live longer than the newly constructed instance
  VoxelizedScene(glrt::scene::Scene& scene);

  void enabledSignedDistanceFields();

private slots:
  void appendSignedDistanceField(glrt::scene::StaticMeshComponent* staticMeshComponent);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_VOXELIZEDSCENE_H
