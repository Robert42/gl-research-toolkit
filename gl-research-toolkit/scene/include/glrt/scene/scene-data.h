#ifndef GLRT_SCENE_SCENE_DATA_H
#define GLRT_SCENE_SCENE_DATA_H

#include "scene.h"
#include <glrt/scene/resources/light-source.h>
#include <glrt/scene/resources/voxel-data.h>
#include <glrt/scene/resources/resource-manager.h>

namespace glrt {
namespace scene {

class Scene::Data
{
public:
  typedef Node::Component::DataClass DataClass;

  template<quint16 _capacity>
  struct DataArray
  {
    static const quint16 arrayCapacity = _capacity;

    static_assert(_capacity>0, "Invalid capacity");

    quint16 capacity() const{return arrayCapacity;}

    quint16 length = 0;
    quint16 numDynamic = 0;
    bool dirtyOrder = false;
    quint8 _padding = 42;

    quint16 last_item_index() const
    {
      Q_ASSERT(length > 0);
      return length - 1;
    }

    static void copy_array_metadata(DataArray<_capacity>* target_data, DataArray<_capacity>* source_data)
    {
      target_data->length = source_data->length;
      target_data->numDynamic = source_data->numDynamic;
      target_data->dirtyOrder = source_data->dirtyOrder;
    }
  };

  template<quint16 capacity>
  struct TransformData : public DataArray<capacity>
  {
    glm::vec3 position[capacity];
    glm::quat orientation[capacity];
    float scaleFactor[capacity];
    CoordFrame local_coord_frame[capacity];
    Node::Component* component[capacity];

    CoordFrame globalCoordFrame(quint32 index) const
    {
      Q_ASSERT(index<DataArray<capacity>::length);
      return CoordFrame(position[index], orientation[index], scaleFactor[index]);
    }

    static void copy_transform_data(TransformData<capacity>* target_data, quint16 target_index, TransformData<capacity>* source_data, quint16 source_index)
    {
      Q_ASSERT(target_index < target_data->length);
      Q_ASSERT(source_index < source_data->length);

      target_data->position[target_index] = source_data->position[source_index];
      target_data->orientation[target_index] = source_data->orientation[source_index];
      target_data->scaleFactor[target_index] = source_data->scaleFactor[source_index];
      target_data->local_coord_frame[target_index] = source_data->local_coord_frame[source_index];
      target_data->component[target_index] = source_data->component[source_index];
      target_data->component[target_index]->data_index.array_index = target_index;
    }

    inline void assert_valid_indices() const
    {
#ifdef QT_DEBUG
      for(quint16 i=0; i<DataArray<capacity>::length; ++i)
        Q_ASSERT(component[i]->data_index.array_index == i);
#endif
    }
  };

  template<quint16 capacity>
  struct LightSourceData : public TransformData<capacity>
  {
    resources::LightSource::CompactAreaLight lightData[capacity];

  protected:
    void swap_light_data(quint16 a, quint16 b)
    {
      lightData[a].swap(lightData[b]);
    }
  };

  template<quint16 capacity>
  struct SphereLightData : public LightSourceData<capacity>
  {
    float radius[capacity];

    float globalRadius(quint16 i) const
    {
      Q_ASSERT(i < LightSourceData<capacity>::length);
      return radius[i] * LightSourceData<capacity>::scaleFactor[i];
    }

    void swap_spherelight_data(quint16 a, quint16 b)
    {
      LightSourceData<capacity>::swap_light_data(a,b);
      std::swap(radius[a], radius[b]);
    }
  };

  template<quint16 capacity>
  struct RectLightData : public LightSourceData<capacity>
  {
    glm::vec2 half_size[capacity];

    glm::vec2 globalHalfSize(quint16 i) const
    {
      Q_ASSERT(i < LightSourceData<capacity>::length);
      return half_size[i] * LightSourceData<capacity>::scaleFactor[i];
    }

    void globalTangents(quint16 i, glm::vec3* tangent1, glm::vec3* tangent2) const
    {
      Q_ASSERT(i < LightSourceData<capacity>::length);
      CoordFrame::_transform_direction(tangent1,
                                       LightSourceData<capacity>::orientation[i],
                                       glm::vec3(-1, 0, 0));
      CoordFrame::_transform_direction(tangent2,
                                       LightSourceData<capacity>::orientation[i],
                                       glm::vec3(0, 1, 0));
    }

    void swap_rectlight_data(quint16 a, quint16 b)
    {
      LightSourceData<capacity>::swap_light_data(a,b);
      std::swap(half_size[a], half_size[b]);
    }
  };

  template<quint16 capacity>
  struct StaticMeshData : public TransformData<capacity>
  {
    typedef TransformData<capacity> parent;

    Uuid<resources::StaticMesh> staticMeshUuid[capacity];
    Uuid<resources::Material> materialUuid[capacity];

    StaticMeshComponent* const * staticMeshComponent(){return reinterpret_cast<StaticMeshComponent* const *>(TransformData<capacity>::component);}

    void swap_staticmesh_data(quint16 a, quint16 b)
    {
      staticMeshUuid[a].swap(staticMeshUuid[b]);
      materialUuid[a].swap(materialUuid[b]);
    }

    static void copy_staticmesh_data(StaticMeshData<capacity>* target_data, quint16 target_index, StaticMeshData<capacity>* source_data, quint16 source_index)
    {
      target_data->staticMeshUuid[target_index] = source_data->staticMeshUuid[source_index];
      target_data->materialUuid[target_index] = source_data->materialUuid[source_index];
    }
  };

  template<quint16 capacity>
  struct VoxelGridData : public TransformData<capacity>
  {
    resources::VoxelData voxelData[capacity];
    resources::BoundingSphere boundingSphere[capacity];
    quint32 z_index[capacity];
    bool voxelizedAsScenery[capacity];

    void swap_voxel_data(quint16 a, quint16 b)
    {
      std::swap(voxelData[a], voxelData[b]);
      std::swap(boundingSphere[a], boundingSphere[b]);
      std::swap(z_index[a], z_index[b]);
      std::swap(voxelizedAsScenery[a], voxelizedAsScenery[b]);
    }

    void aabb_for(AABB* target_aabb, quint16 i) const
    {
      CoordFrame coordFrame = TransformData<capacity>::globalCoordFrame(i) * voxelData[i].localToVoxelSpace.inverse();
      glm::vec3 voxelSize = glm::vec3(voxelData[i].voxelCount);

      glm::vec3 a = coordFrame.transform_point(glm::vec3(0));
      glm::vec3 b = coordFrame.transform_point(voxelSize);

      *target_aabb |= a;
      *target_aabb |= b;
    }
  };

  template<quint16 capacity>
  struct VoxelBVH : public DataArray<capacity>
  {
  };

  template<quint16 capacity>
  struct CameraData : public TransformData<capacity>
  {
    void swap_camera_data(quint16 a, quint16 b)
    {
      Q_UNUSED(a);
      Q_UNUSED(b);
    }
  };

  struct Transformations
  {
    const quint16 capacity;
    padding<quint16, 3> _padding;
    quint16& length;
    quint16& numDynamic;
    bool& dirtyOrder;
    glm::vec3* const position;
    glm::quat* const orientation;
    float* const scaleFactor;
    CoordFrame* const local_coord_frame;
    Node::Component** const component;

    CoordFrame globalCoordFrame(quint16 index) const
    {
      Q_ASSERT(index<length);
      return CoordFrame(position[index], orientation[index], scaleFactor[index]);
    }

    template<quint16 c>
    Transformations(TransformData<c>& data)
      : capacity(data.capacity()),
        length(data.length),
        numDynamic(data.numDynamic),
        dirtyOrder(data.dirtyOrder),
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

      std::swap(component[a]->data_index.array_index, component[b]->data_index.array_index);

      std::swap(position[a], position[b]);
      std::swap(orientation[a], orientation[b]);
      std::swap(scaleFactor[a], scaleFactor[b]);
      std::swap(local_coord_frame[a], local_coord_frame[b]);
      std::swap(component[a], component[b]);
    }

    quint16 last_item_index() const
    {
      Q_ASSERT(length > 0);
      return length - 1;
    }
  };

  typedef SphereLightData<0xffff> SphereLights;
  typedef RectLightData<0x100> RectLights;
  typedef StaticMeshData<0xffff> StaticMeshes;
  typedef VoxelGridData<0xffff> VoxelGrids;
  typedef VoxelBVH<VoxelGrids::arrayCapacity-1> VoxelBVHs;
  typedef CameraData<0x100> Cameras;

  TransformData<0xffff> emptyNodes;
  SphereLights sphereLights;
  RectLights rectLights;
  StaticMeshes* staticMeshes = &staticMeshes1;
  VoxelGrids voxelGrids;
  Cameras cameras;

  VoxelBVHs voxelBVH;

  Scene& scene;

  static const quint32 numTransformations = quint32(DataClass::NUM_DATA_CLASSES);
  Transformations* transformations[numTransformations];

  Transformations& transformDataForClass(Node::Component::DataClass dataClass);
  Transformations& transformDataForIndex(Node::Component::DataIndex dataIndex);

  const Transformations& transformDataForClass(Node::Component::DataClass dataClass) const;
  const Transformations& transformDataForIndex(Node::Component::DataIndex dataIndex) const;


  Data(Scene& scene);
  ~Data();

  void sort_staticMeshes();

private:
  // static_mesh double buffering
  StaticMeshes* staticMeshes_backbuffer = &staticMeshes2;
  Transformations* transformation_staticMeshes_backbuffer;
  StaticMeshes staticMeshes1;
  StaticMeshes staticMeshes2;
  quint16 static_mesh_index_reorder[StaticMeshes::arrayCapacity];
};


inline Scene::Data::Transformations& Scene::Data::transformDataForClass(Node::Component::DataClass dataClass)
{
  quint8 class_index = quint8(dataClass & Node::Component::DataClass::MASK);

  Q_ASSERT(class_index < numTransformations);

  return *transformations[class_index];
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

  return *transformations[class_index];
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
