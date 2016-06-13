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

  struct VoxelDataBlock
  {
    glm::vec3 globalWorldToVoxelMatrix_col0;
    int voxelCount_x;
    glm::vec3 globalWorldToVoxelMatrix_col1;
    int voxelCount_y;
    glm::vec3 globalWorldToVoxelMatrix_col2;
    int voxelCount_z;
    glm::vec3 globalWorldToVoxelMatrix_col3;
    float globalWorldToVoxelFactor;

    quint64 texture;
    padding<quint64, 1> _padding1;
  };


  Data data;
  bool voxelizedAsScenery = false;

  VoxelDataComponent(Node& node, Node::Component* parent, const Uuid<VoxelDataComponent>& uuid, const Data& data=Data(), bool voxelizedAsScenery=false);

  glm::mat4x3 globalWorldToVoxelMatrix4x3() const;
  glm::mat4 globalWorldToVoxelMatrix4() const;
  glm::ivec3 gridSize() const;
  VoxelDataBlock voxelDataBlock() const;
  quint64 textureData() const;
  resources::BoundingSphere boundingSphere() const;
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_VOXELDATACOMPONENT_H
