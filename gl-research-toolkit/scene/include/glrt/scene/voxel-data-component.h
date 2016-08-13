#ifndef GLRT_SCENE_VOXELDATACOMPONENT_H
#define GLRT_SCENE_VOXELDATACOMPONENT_H

#include <glrt/scene/node.h>
#include <glrt/scene/resources/voxel-data.h>

namespace glrt {
namespace scene {

class VoxelDataComponent : public glrt::scene::Node::Component
{
public:
  typedef resources::VoxelData Data;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data=Data(), bool voxelizedAsScenery=false);
  ~VoxelDataComponent();
  glm::mat4x3 globalWorldToVoxelMatrix4x3() const;
  glm::mat4 globalWorldToVoxelMatrix4() const;
  glm::ivec3 gridSize() const;
  quint64 textureData() const;
  resources::BoundingSphere boundingSphere() const;
  const Data& data() const;
  bool voxelizedAsScenery() const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
