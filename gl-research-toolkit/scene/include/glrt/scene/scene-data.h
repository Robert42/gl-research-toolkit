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
    quint16 firstDynamic = 0;
    quint16 firstDirty = 0;
    glm::vec3 position[arrayCapacity];
    glm::quat orientation[arrayCapacity];
    float scaleFactor[arrayCapacity];
    CoordFrame local_coord_frame[arrayCapacity];
    quint32 z_index[arrayCapacity];
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
    const quint32 capacity;
    padding<quint32, 1> _padding;
    quint32& length;
    quint16& firstDynamic;
    quint16& firstDirty;
    glm::vec3* const position;
    glm::quat* const orientation;
    float* const scaleFactor;
    CoordFrame* const local_coord_frame;
    Node::Component** const component;

    CoordFrame globalCoordFrame(quint32 index) const
    {
      Q_ASSERT(index<length);
      return CoordFrame(position[index], orientation[index], scaleFactor[index]);
    }

    template<quint32 c>
    Transformations(TransformData<c>& data)
      : capacity(data.capacity()),
        length(data.length),
        firstDynamic(data.firstDynamic),
        firstDirty(data.firstDirty),
        position(data.position),
        orientation(data.orientation),
        scaleFactor(data.scaleFactor),
        local_coord_frame(data.local_coord_frame),
        component(data.component)
    {
    }

    void swap_transform_data(quint16 a, quint16 b)
    {
      Q_ASSERT(a < length);
      Q_ASSERT(b < length);

      std::swap(position[a], position[b]);
      std::swap(orientation[a], orientation[b]);
      std::swap(scaleFactor[a], scaleFactor[b]);
      std::swap(local_coord_frame[a], local_coord_frame[b]);
      std::swap(component[a], component[b]);
    }

    quint16 last_item_index() const
    {
      Q_ASSERT(length <= 0x10000);
      return static_cast<quint16>(length - 1);
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
  Transformations& transformDataForIndex(Node::Component::DataIndex dataIndex);

  const Transformations& transformDataForClass(Node::Component::DataClass dataClass) const;
  const Transformations& transformDataForIndex(Node::Component::DataIndex dataIndex) const;


  Data();
};


inline Scene::Data::Transformations& Scene::Data::transformDataForClass(Node::Component::DataClass dataClass)
{
  quint8 class_index = quint8(dataClass & Node::Component::DataClass::MASK);

  Q_ASSERT(class_index < numTransformations);

  return transformations[class_index];
}

inline Scene::Data::Transformations& Scene::Data::transformDataForIndex(Node::Component::DataIndex dataIndex)
{
  Transformations& transformations = Scene::Data::transformDataForClass(dataIndex.data_class);

  Q_ASSERT(dataIndex.array_index < transformations.length);

  return transformations;
}


inline const Scene::Data::Transformations& Scene::Data::transformDataForClass(Node::Component::DataClass dataClass) const
{
  quint8 class_index = quint8(dataClass & Node::Component::DataClass::MASK);

  Q_ASSERT(class_index < numTransformations);

  return transformations[class_index];
}

inline const Scene::Data::Transformations& Scene::Data::transformDataForIndex(Node::Component::DataIndex dataIndex) const
{
  const Transformations& transformations = Scene::Data::transformDataForClass(dataIndex.data_class);

  Q_ASSERT(dataIndex.array_index < transformations.length);

  return transformations;
}


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_DATA_H
