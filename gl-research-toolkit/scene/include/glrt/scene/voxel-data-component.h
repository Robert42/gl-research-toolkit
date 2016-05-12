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

  struct GridSize
  {
    glm::ivec3 voxelCount;
    padding<int, 1> _padding;
  };

  struct WorldVoxelUvwSpaceFactor
  {
    glm::vec3 voxelToUvwSpace;
    float voxelToWorldSpace;
  };


  Data data;
  bool voxelizedAsScenery = false;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data=Data(), bool voxelizedAsScenery=false);

  glm::mat4 globalWorldToVoxelMatrix() const;
  GridSize gridSize() const;
  WorldVoxelUvwSpaceFactor spaceFactor() const;
  quint64 textureData() const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
