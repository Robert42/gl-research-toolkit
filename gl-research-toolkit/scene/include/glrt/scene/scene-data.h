#ifndef GLRT_SCENE_SCENE_DATA_H
#define GLRT_SCENE_SCENE_DATA_H

#include "scene.h"
#include <glrt/toolkit/bit-magic.h>
#include <glrt/scene/resources/light-source.h>
#include <glrt/scene/resources/resource-manager.h>

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

    quint16 last_item_index() const
    {
      Q_ASSERT(length <= 0x10000);
      return static_cast<quint16>(length - 1);
    }
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

  protected:
    void swap_light_data(quint16 a, quint16 b)
    {
      lightData[a].swap(lightData[b]);
    }
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

    void swap_spherelight_data(quint16 a, quint16 b)
    {
      LightSourceData<capacity>::swap_light_data(a,b);
      std::swap(radius[a], radius[b]);
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

  template<quint32 capacity>
  struct MeshMaterialCombinationData : public DataArray<capacity>
  {
    typedef DataArray<capacity> parent;

    Uuid<resources::StaticMesh>* static_mesh_uuid = _static_mesh_uuid1;
    Uuid<resources::Material>* material_uuid = _material_uuid1;

    // After sorting, this array holds the index, where each uuid was moved to
    quint16 sorting_index_redirection[capacity];


    void sort(resources::ResourceManager& resourceManager)
    {
#pragma omp parallel for
      for(quint16 i=0; i<parent::length; ++i)
        sorting_index[i] = i;

      auto sorting_order = [this, &resourceManager](quint16 a, quint16 b){
              const Uuid<resources::Material>& material_uuid_a = material_uuid[a];
              const Uuid<resources::Material>& material_uuid_b = material_uuid[b];

              return concatenated_lessThan(one_if_zero(usage_count[a]),
                                           one_if_zero(usage_count[b]),
                                           resourceManager.materialForUuid(material_uuid_a),
                                           resourceManager.materialForUuid(material_uuid_b),
                                           material_uuid_a,
                                           material_uuid_b,
                                           static_mesh_uuid[a],
                                           static_mesh_uuid[b]);
      };

      std::sort(&sorting_index[0], &sorting_index[parent::length], sorting_order);

#pragma omp parallel for
      for(quint16 i=0; i<parent::length; ++i)
      {
        back_buffer_material[i] = material_uuid[sorting_index[i]];
        back_buffer_mesh[i] = static_mesh_uuid[sorting_index[i]];
        sorting_index_redirection[sorting_index[i]] = i;
      }

      std::swap(back_buffer_material, material_uuid);
      std::swap(back_buffer_mesh, static_mesh_uuid);

#ifdef QT_DEBUG
      std::sort(&sorting_index[0], &sorting_index[parent::length], sorting_order);
      for(quint16 i=0; i<parent::length; ++i)
        Q_ASSERT(sorting_index[i] == i);
#endif

      parent::dirtyOrder = false;
    }

    quint16 add(const Uuid<resources::StaticMesh>& staticMesh, const Uuid<resources::Material>& material)
    {
      for(quint16 i=0; i<parent::length; i++)
      {
        if(Q_UNLIKELY(static_mesh_uuid[i] == staticMesh && material_uuid[i] == material))
        {
          usage_count[i]++;
          return i;
        }
      }

      Q_ASSERT(parent::length < parent::arrayCapacity);
      parent::length++;
      quint16 index = parent::last_item_index();

      usage_count[index] = 1;
      static_mesh_uuid[index] = staticMesh;
      material_uuid[index] = material;

      parent::dirtyOrder = true;

      return index;
    }

    void remove(quint16 index)
    {
      Q_ASSERT(index < parent::length);
      usage_count[index]--;
      parent::dirtyOrder = parent::dirtyOrder || usage_count[index] == 0;
    }

  private:
    quint16 usage_count[capacity];

    // After sorting, this array holds the index, from where each uuid was moved from
    quint16 sorting_index[capacity];

    Uuid<resources::StaticMesh>* back_buffer_mesh = _static_mesh_uuid2;
    Uuid<resources::Material>* back_buffer_material = _material_uuid2;

    Uuid<resources::StaticMesh> _static_mesh_uuid1[capacity];
    Uuid<resources::StaticMesh> _static_mesh_uuid2[capacity];
    Uuid<resources::Material> _material_uuid1[capacity];
    Uuid<resources::Material> _material_uuid2[capacity];
  };

  template<quint32 capacity>
  struct StaticMeshData : public TransformData<capacity>
  {
    typedef TransformData<capacity> parent;

    MeshMaterialCombinationData<capacity>& meshMaterialCombinations;
    quint16 redirection_index[capacity];

    StaticMeshData(Scene::Data::MeshMaterialCombinationData<capacity>& meshMaterialCombinations)
      : meshMaterialCombinations(meshMaterialCombinations)
    {
    }

    void push_static_mesh_target(const Uuid<resources::StaticMesh>& staticMesh, const Uuid<resources::Material>& material)
    {
      const quint16 meshMaterialIndex = meshMaterialCombinations.add(staticMesh, material);
      const quint16 index = parent::last_item_index();

      redirection_index[index] = meshMaterialIndex;
    }

    void pop_static_mesh_target()
    {
      const quint16 index = parent::last_item_index();

      Q_ASSERT(index<parent::length);

      meshMaterialCombinations.remove(index);
    }

    void swap_staticmesh_data(quint16 a, quint16 b)
    {
      std::swap(redirection_index[a], redirection_index[b]);
    }

    bool is_reorder_necessary() const
    {
      return meshMaterialCombinations.dirtyOrder;
    }

    void reorder()
    {
      meshMaterialCombinations.sort();
#pragma omp parallel for
      for(quint16 i=0; i<parent::length; ++i)
        redirection_index[i] = meshMaterialCombinations.sorting_index_redirection[i];
    }
  };

  template<quint32 capacity>
  struct VoxelGridData : public TransformData<capacity>
  {
    void swap_voxel_data(quint16 a, quint16 b)
    {
      Q_UNUSED(a);
      Q_UNUSED(b);
    }
  };

  template<quint32 capacity>
  struct VoxelBVH : public DataArray<capacity>
  {
  };

  template<quint32 capacity>
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
  typedef MeshMaterialCombinationData<StaticMeshes::arrayCapacity> MeshMaterialCombinations;
  typedef VoxelGridData<0x10000> VoxelGrids;
  typedef VoxelBVH<VoxelGrids::arrayCapacity> VoxelBVHs;
  typedef CameraData<0x100> Cameras;

  TransformData<0x10000> emptyNodes;
  SphereLights sphereLights;
  RectLights rectLights;
  StaticMeshes staticMeshes;
  MeshMaterialCombinations meshMaterialCombinations;
  VoxelGrids voxelGrids;
  Cameras cameras;

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
