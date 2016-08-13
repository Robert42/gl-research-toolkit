#include <glrt/scene/global-coord-updater.h>
#include <glrt/scene/scene.h>
#include <glrt/scene/scene-data.h>
#include <glrt/toolkit/concatenated-less-than.h>
#include <glrt/toolkit/bit-magic.h>
#include <glrt/toolkit/profiler.h>

namespace glrt {
namespace scene {


GlobalCoordUpdater::GlobalCoordUpdater(Scene* scene)
  : scene(*scene)
{
}

GlobalCoordUpdater::~GlobalCoordUpdater()
{
  for(const Array<Node::Component*>& a : notDynamicComponents_pending)
    Q_ASSERT(a.isEmpty());
  for(const Array<Node::Component*>& a : dynamicComponents)
    Q_ASSERT(a.isEmpty());
}


void GlobalCoordUpdater::addComponent(Node::Component* component)
{
  component->updateCoordDependencyDepth();

  Array<Node::Component*>& array = arrayFor(component);

  array.append(component);

  needResortingFor(component);
}

void GlobalCoordUpdater::removeComponent(Node::Component* component)
{
  Array<Node::Component*>& array = arrayFor(component);

  int index = array.indexOfFirst(component, -1);
  Q_ASSERT(index != -1);
  array.removeAt(index);

  needResortingFor(component);
}


void GlobalCoordUpdater::updateCoordinates()
{
  PROFILE_SCOPE("GlobalCoordUpdater::updateCoordinates()");

  if(Q_UNLIKELY(_need_resorting_not_dynamic!=0))
  {
    PROFILE_SCOPE("GlobalCoordUpdater _need_resorting_not_dynamic!=0");
    resort(&notDynamicComponents_pending, &_need_resorting_not_dynamic); // PERFORMANCE is this increasing performance?
    updateCoordinatesOf(notDynamicComponents_pending);
  }

  if(Q_UNLIKELY(_need_resorting_dynamic!=0))
  {
    PROFILE_SCOPE("GlobalCoordUpdater _need_resorting_dynamic!=0");
    resort(&dynamicComponents, &_need_resorting_dynamic);
  }

  updateCoordinatesOf(dynamicComponents);
}

inline bool componentDataIndexOrder(const Node::Component* a, const Node::Component* b)
{
  return concatenated_lessThan(a->data_index.data_class, b->data_index.data_class,
                               a->data_index.array_index, b->data_index.array_index,
                               a->parent!=nullptr ? quint32(a->parent->data_index.data_class) : 0, b->parent!=nullptr ? quint32(b->parent->data_index.data_class) : 0,
                               a->parent!=nullptr ? a->parent->data_index.array_index : 0, b->parent!=nullptr ? b->parent->data_index.array_index : 0);
}

void GlobalCoordUpdater::resort(Array<Array<Node::Component*>>* arrays, GlobalCoordUpdater::Bitfield* needResorting)
{
  int highestDepth = static_cast<int>(bitIndexOf<quint64>(glm::highestBitValue(*needResorting)));
  int lowestDepth = static_cast<int>(bitIndexOf<quint64>(glm::lowestBitValue(*needResorting)));

  Q_ASSERT(glm::highestBitValue(*needResorting) == (quint64(1)<<quint64(highestDepth)));
  Q_ASSERT(glm::lowestBitValue(*needResorting) == (quint64(1)<<quint64(lowestDepth)));

  for(int depth = lowestDepth; depth<=highestDepth; ++depth)
  {
    Bitfield bitfield = quint64(1)<<quint64(depth);
    if(Q_UNLIKELY(bitfield & *needResorting))
    {
      arrays->at(depth).sort(componentDataIndexOrder);

      *needResorting &= ~bitfield;
    }
  }

  Q_ASSERT(*needResorting == 0);
}

void GlobalCoordUpdater::updateCoordinatesOf(Array<Array<Node::Component*>>& arrays)
{
  PROFILE_SCOPE("GlobalCoordUpdater::updateCoordinatesOf(Array<Array<>>)");

  if(Q_UNLIKELY(arrays.isEmpty()))
    return;

  copyLocalToGlobalCoordinates(arrays.first());

  for(int depth=1; depth<arrays.length(); ++depth)
    updateCoordinatesOf(arrays.at(depth));
}

void GlobalCoordUpdater::copyLocalToGlobalCoordinates(const Array<Node::Component*>& array)
{
  PROFILE_SCOPE("GlobalCoordUpdater::copyLocalToGlobalCoordinates");
  const int n = array.length();
  Scene::Data& sceneData = *scene.data;

// #pragma omp parallel for
  for(int i=0; i<n; ++i)
  {
    Q_ASSERT(array[i]->parent == nullptr);

    Node::Component::DataIndex data_index = array[i]->data_index;

    Scene::Data::Transformations& transformations = sceneData.transformDataForIndex(data_index);
    const quint32 array_index = data_index.array_index;

    transformations.position[array_index] = transformations.local_coord_frame[array_index].position;
    transformations.orientation[array_index] = transformations.local_coord_frame[array_index].orientation;
    transformations.scaleFactor[array_index] = transformations.local_coord_frame[array_index].scaleFactor;
    // transformations.z_index[array_index] = scene.aabb.toUnitSpace(); TODO calc z_index
  }
}

void GlobalCoordUpdater::updateCoordinatesOf(const Array<Node::Component*>& array)
{
  PROFILE_SCOPE("GlobalCoordUpdater::updateCoordinatesOf(Array<>)");
  const int n = array.length();
  Scene::Data& sceneData = *scene.data;

//  #pragma omp parallel for
  for(int i=0; i<n; ++i)
  {
    Q_ASSERT(array[i]->parent != nullptr);

    Node::Component::DataIndex data_index = array[i]->data_index;
    Node::Component::DataIndex parent_data_index = array[i]->parent->data_index;

    Scene::Data::Transformations& transformations = sceneData.transformDataForIndex(data_index);
    Scene::Data::Transformations& transformations_parent = sceneData.transformDataForIndex(parent_data_index);
    const quint16 array_index = data_index.array_index;
    const quint16 parent_array_index = parent_data_index.array_index;

    CoordFrame global_coord = transformations_parent.globalCoordFrame(parent_array_index) * transformations.local_coord_frame[array_index];

    transformations.position[array_index] = global_coord.position;
    transformations.orientation[array_index] = global_coord.orientation;
    transformations.scaleFactor[array_index] = global_coord.scaleFactor;
    // transformations.z_index[array_index] = scene.aabb.toUnitSpace(); TODO calc z_index
  }
}

Array<Node::Component*>& GlobalCoordUpdater::arrayFor(Node::Component* component)
{
  Array<Array<Node::Component*>>& arrays = component->isDynamic() ? dynamicComponents : notDynamicComponents_pending;

  int dependencyDepth = component->coordDependencyDepth();

  while(dependencyDepth>=arrays.length())
    arrays.append(std::move(Array<Node::Component*>()));

  return arrays[dependencyDepth];
}

void GlobalCoordUpdater::needResortingFor(Node::Component* component)
{
  quint64& need_resorting = component->isDynamic() ? _need_resorting_dynamic : _need_resorting_not_dynamic;

  int dependencyDepth = component->coordDependencyDepth();

  Q_ASSERT(dependencyDepth>=0 && dependencyDepth<64);
  need_resorting |= 1<<dependencyDepth;
}

} // namespace scene
} // namespace glrt
