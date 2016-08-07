#ifndef GLRT_SCENE_SCENE_DATA_H
#define GLRT_SCENE_SCENE_DATA_H

#include "scene.h"
#include <glrt/scene/resources/light-source.h>

namespace glrt {
namespace scene {

class Scene::Data
{
public:
  typedef Node::Component::DataClass DataClass;

  template<quint32 _capacity>
  struct DataArray
  {
    static const quint32 arrayCapacity = _capacity;

    static_assert(arrayCapacity>0 && arrayCapacity<=65536, "Invalid capacity");

    quint32 capacity() const{return arrayCapacity;}

    quint32 length = 0;
    quint16 numDynamic = 0;
    bool dirtyOrder = false;
    quint8 _padding = 42;
  };

  template<quint32 capacity>
  struct TransformData : public DataArray<capacity>
  {
    glm::vec3 position[capacity];
    glm::quat orientation[capacity];
    float scaleFactor[capacity];
    CoordFrame local_coord_frame[capacity];
    quint32 z_index[capacity];
    Node::Component* component[capacity];

    CoordFrame globalCoordFrame(quint32 index) const
    {
      Q_ASSERT(index<DataArray<capacity>::length);
      return CoordFrame(position[index], orientation[index], scaleFactor[index]);
    }
  };

  template<quint32 capacity>
  struct LightSourceData : public TransformData<capacity>
  {
    resources::LightSource::CompactAreaLight lightData[capacity];
  };

  template<quint32 capacity>
  struct SphereLightData : public LightSourceData<capacity>
  {
    float radius[capacity];

    float globalRadius(quint32 i) const
    {
      Q_ASSERT(i < LightSourceData<capacity>::length);
      return radius[i] * LightSourceData<capacity>::scaleFactor[i];
    }
  };

  template<quint32 capacity>
  struct RectLightData : public LightSourceData<capacity>
  {
    glm::vec2 half_size[capacity];

    glm::vec2 globalHalfSize(quint32 i) const
    {
      Q_ASSERT(i < LightSourceData<capacity>::length);
      return half_size[i] * LightSourceData<capacity>::scaleFactor[i];
    }

    void globalTangents(quint32 i, glm::vec3* tangent1, glm::vec3* tangent2) const
    {
      Q_ASSERT(i < LightSourceData<capacity>::length);
      CoordFrame::_transform_direction(tangent1,
                                       LightSourceData<capacity>::orientation[i],
                                       glm::vec3(1, 0, 0));
      CoordFrame::_transform_direction(tangent2,
                                       LightSourceData<capacity>::orientation[i],
                                       glm::vec3(0, 1, 0));
    }
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
  struct VoxelBVH : public DataArray<capacity>
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
    quint16& numDynamic;
    bool& dirtyOrder;
    glm::vec3* const position;
    glm::quat* const orientation;
    float* const scaleFactor;
    CoordFrame* const local_coord_frame;
    quint32* const z_index;
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
        numDynamic(data.numDynamic),
        dirtyOrder(data.dirtyOrder),
        position(data.position),
        orientation(data.orientation),
        scaleFactor(data.scaleFactor),
        local_coord_frame(data.local_coord_frame),
        z_index(data.z_index),
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
      std::swap(z_index[a], z_index[b]);
      std::swap(component[a], component[b]);
    }

    quint16 last_item_index() const
    {
      Q_ASSERT(length <= 0x10000);
      return static_cast<quint16>(length - 1);
    }
  };

  typedef SphereLightData<0x10000> SphereLights;
  typedef RectLightData<0x100> RectLights;
  typedef StaticMeshData<0x10000> StaticMeshes;
  typedef VoxelGridData<0x10000> VoxelGrids;
  typedef VoxelBVH<VoxelGrids::arrayCapacity> VoxelBVHs;

  TransformData<0x10000> emptyNodes;
  SphereLights sphereLights;
  RectLights rectLights;
  StaticMeshes staticMeshes;
  VoxelGrids voxelGridData;
  CameraData<0x100> cameras;

  VoxelBVHs voxelBVH;

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
