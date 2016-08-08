#include <glrt/scene/scene-data.h>
#include <glrt/toolkit/concatenated-less-than.h>

namespace glrt {
namespace scene {

Scene::Data::Data(resources::ResourceManager& resourceManager)
  : resourceManager(resourceManager),
    transformations{new Transformations(emptyNodes), new Transformations(sphereLights), new Transformations(rectLights), new Transformations(*staticMeshes), new Transformations(voxelGrids), new Transformations(cameras)},
    transformation_staticMeshes_backbuffer(new Transformations(staticMeshes2))
{
  Q_ASSERT(transformations[quint32(DataClass::EMPTY)]->position ==  emptyNodes.position);
  Q_ASSERT(transformations[quint32(DataClass::SPHERELIGHT)]->position ==  sphereLights.position);
  Q_ASSERT(transformations[quint32(DataClass::RECTLIGHT)]->position ==  rectLights.position);
  Q_ASSERT(transformations[quint32(DataClass::STATICMESH)]->position ==  staticMeshes->position);
  Q_ASSERT(transformations[quint32(DataClass::VOXELGRID)]->position ==  voxelGrids.position);
  Q_ASSERT(transformations[quint32(DataClass::CAMERA)]->position ==  cameras.position);

  Q_ASSERT(transformation_staticMeshes_backbuffer->position ==  staticMeshes_backbuffer->position);
}

Scene::Data::~Data()
{
  for(quint32 i=0; i<numTransformations; ++i)
    delete transformations[i];

  delete transformation_staticMeshes_backbuffer;
}

void Scene::Data::sort_staticMeshes()
{
#pragma omp parallel for
  for(quint16 i=0; i<staticMeshes->length; ++i)
    static_mesh_index_reorder[i] = i;

  auto sorting_order = [this](quint16 a, quint16 b){
          const Uuid<resources::Material>& material_uuid_a = staticMeshes->materialUuid[a];
          const Uuid<resources::Material>& material_uuid_b = staticMeshes->materialUuid[b];

          return concatenated_lessThan(resourceManager.materialForUuid(material_uuid_a),
                                       resourceManager.materialForUuid(material_uuid_b),
                                       material_uuid_a,
                                       material_uuid_b,
                                       staticMeshes->staticMeshUuid[a],
                                       staticMeshes->staticMeshUuid[b],
                                       staticMeshes->z_index[a],
                                       staticMeshes->z_index[b]);
  };

  std::sort(&static_mesh_index_reorder[0], &static_mesh_index_reorder[staticMeshes->length], sorting_order);

  StaticMeshes::copy_array_metadata(staticMeshes_backbuffer, staticMeshes);
#pragma omp parallel for
  for(quint16 i=0; i<staticMeshes->length; ++i)
  {
    StaticMeshes::copy_transform_data(staticMeshes_backbuffer, i, staticMeshes, static_mesh_index_reorder[i]);
    StaticMeshes::copy_staticmesh_data(staticMeshes_backbuffer, i, staticMeshes, static_mesh_index_reorder[i]);
  }

  std::swap(staticMeshes, staticMeshes_backbuffer);
  std::swap(transformation_staticMeshes_backbuffer, transformations[static_cast<quint32>(DataClass::STATICMESH)]);

#ifdef QT_DEBUG
  std::sort(&static_mesh_index_reorder[0], &static_mesh_index_reorder[staticMeshes->length], sorting_order);
  for(quint16 i=0; i<staticMeshes->length; ++i)
    Q_ASSERT(static_mesh_index_reorder[i] == i);
#endif

  staticMeshes->dirtyOrder = false;
}

} // namespace scene
} // namespace glrt
