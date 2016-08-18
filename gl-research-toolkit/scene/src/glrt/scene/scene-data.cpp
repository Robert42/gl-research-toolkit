#include <glrt/scene/scene-data.h>
#include <glrt/toolkit/concatenated-less-than.h>

namespace glrt {
namespace scene {

Scene::Data::Data(Scene& scene)
  : scene(scene),
    transformations{new Transformations(emptyNodes), new Transformations(sphereLights), new Transformations(rectLights), new Transformations(staticMeshes1), new Transformations(voxelGrids), new Transformations(cameras)},
    transformation_staticMeshes_backbuffer(new Transformations(staticMeshes2))
{
  Q_ASSERT(transformations[quint32(DataClass::EMPTY)]->position ==  emptyNodes.position);
  Q_ASSERT(transformations[quint32(DataClass::SPHERELIGHT)]->position ==  sphereLights.position);
  Q_ASSERT(transformations[quint32(DataClass::RECTLIGHT)]->position ==  rectLights.position);
  Q_ASSERT(transformations[quint32(DataClass::STATICMESH)]->position ==  staticMeshes->position);
  Q_ASSERT(transformations[quint32(DataClass::VOXELGRID)]->position ==  voxelGrids.position);
  Q_ASSERT(transformations[quint32(DataClass::CAMERA)]->position ==  cameras.position);

  Q_ASSERT(transformation_staticMeshes_backbuffer->position ==  staticMeshes_backbuffer->position);

  clear();
}

Scene::Data::~Data()
{
  for(quint32 i=0; i<numTransformations; ++i)
    delete transformations[i];

  delete transformation_staticMeshes_backbuffer;
}

void Scene::Data::clear()
{
  for(uint32_t i=0; i<numTransformations; ++i)
    Q_ASSERT(transformations[i]->length == 0);

  const uint16_t n = voxelGrids.capacity();
  resources::BoundingSphere* spheres = voxelGrids.boundingSphere;
#pragma omp for simd
  for(int i=0; i<n; ++i)
  {
    spheres[i].center = glm::vec3(NAN);
    spheres[i].radius = NAN;
  }

}

void Scene::Data::sort_staticMeshes()
{
  staticMeshes->assert_valid_indices();

#pragma omp simd
  for(quint16 i=0; i<staticMeshes->length; ++i)
    static_mesh_index_reorder[i] = i;

  const resources::ResourceManager& resourceManager = scene.resourceManager;

  auto sorting_order = [this,&resourceManager](quint16 a, quint16 b){
          const Uuid<resources::Material>& material_uuid_a = staticMeshes->materialUuid[a];
          const Uuid<resources::Material>& material_uuid_b = staticMeshes->materialUuid[b];

          return concatenated_lessThan(resourceManager.materialForUuid(material_uuid_a),
                                       resourceManager.materialForUuid(material_uuid_b),
                                       material_uuid_a,
                                       material_uuid_b,
                                       staticMeshes->staticMeshUuid[a],
                                       staticMeshes->staticMeshUuid[b]);
          // PERFORMANCE: also sort by zindex for better cache utilzation
  };

  std::stable_sort(&static_mesh_index_reorder[0], &static_mesh_index_reorder[staticMeshes->length], sorting_order);

  StaticMeshes::copy_array_metadata(staticMeshes_backbuffer, staticMeshes);
#pragma omp simd
  for(quint16 i=0; i<staticMeshes->length; ++i)
  {
    StaticMeshes::copy_transform_data(staticMeshes_backbuffer, i, staticMeshes, static_mesh_index_reorder[i]);
    StaticMeshes::copy_staticmesh_data(staticMeshes_backbuffer, i, staticMeshes, static_mesh_index_reorder[i]);
  }

  std::swap(staticMeshes, staticMeshes_backbuffer);
  std::swap(transformation_staticMeshes_backbuffer, transformations[static_cast<quint32>(DataClass::STATICMESH)]);

#ifdef QT_DEBUG
  staticMeshes->assert_valid_indices();
  #pragma omp simd
    for(quint16 i=0; i<staticMeshes->length; ++i)
      static_mesh_index_reorder[i] = i;
  Q_ASSERT(std::is_sorted(&static_mesh_index_reorder[0], &static_mesh_index_reorder[staticMeshes->length], sorting_order));
#endif

  staticMeshes->dirtyOrder = false;
}

} // namespace scene
} // namespace glrt
