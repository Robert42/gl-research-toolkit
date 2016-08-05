#ifndef GLRT_SCENE_SCENE_DATA_H
#define GLRT_SCENE_SCENE_DATA_H

#include "scene.h"

namespace glrt {
namespace scene {

class Scene::Data
{
public:
  typedef Node::Component::DataClass DataClass;

  template<quint32 _capacity>
  struct TransformData
  {
    static const quint32 arrayCapacity = _capacity;

    static_assert(arrayCapacity>0 && arrayCapacity<=65536, "Invalid capacity");

    quint32 capacity() const{return arrayCapacity;}

    quint32 length = 0;
    // TODO: performance increase, when adding the capacity as member for better alignment?
    glm::vec3 position[arrayCapacity];
    glm::quat orientation[arrayCapacity];
    float scaleFactor[arrayCapacity];
    CoordFrame local_coord_frame[arrayCapacity];
    Node::Component* component[arrayCapacity];
  };

  template<quint32 capacity>
  struct LightSourceData : public TransformData<capacity>
  {
    glm::vec3 color[capacity];
    float luminous_power[capacity];
    float influence_radius[capacity];
  };

  template<quint32 capacity>
  struct SphereLightData : public LightSourceData<capacity>
  {
    float radius[capacity];
  };

  template<quint32 capacity>
  struct RectLightData : public LightSourceData<capacity>
  {
    float radius[capacity];
  };

  template<quint32 capacity>
  struct StaticMeshData : public TransformData<capacity>
  {
  };

  template<quint32 capacity>
  struct VoxelGridData : public TransformData<capacity>
  {
  };

  template<quint32 capacity>
  struct CameraData : public TransformData<capacity>
  {
  };

  struct Transformations
  {
    quint32& length;
    glm::vec3* const position;
    glm::quat* const orientation;
    float* const scaleFactor;
    CoordFrame* const local_coord_frame;
    Node::Component** const component;

    const quint32 capacity;
    padding<quint32, 1> _padding;

    template<quint32 c>
    Transformations(TransformData<c>& data)
      : length(data.length),
        position(data.position),
        orientation(data.orientation),
        scaleFactor(data.scaleFactor),
        local_coord_frame(data.local_coord_frame),
        component(data.component),
        capacity(data.capacity())
    {
    }
  };

  TransformData<0x10000> emptyNodes;
  SphereLightData<0x10000> sphereLightData;
  RectLightData<0x100> rectLightData;
  StaticMeshData<0x10000> staticMeshData;
  VoxelGridData<0x10000> voxelGridData;
  CameraData<0x100> cameras;

  static const quint32 numTransformations = quint32(DataClass::NUM_DATA_CLASSES);
  Transformations transformations[numTransformations];

  Transformations& transformDataForClass(Node::Component::DataClass dataClass);


  Data();
};


inline Scene::Data::Transformations& Scene::Data::transformDataForClass(Node::Component::DataClass dataClass)
{

  quint8 class_index = quint8(dataClass & Node::Component::DataClass::MASK);

  Q_ASSERT(class_index < numTransformations);

  return transformations[class_index];
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_DATA_H
