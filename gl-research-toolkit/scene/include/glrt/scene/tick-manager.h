#ifndef GLRT_SCENE_TICKMANAGER_H
#define GLRT_SCENE_TICKMANAGER_H

#include <glrt/scene/ticking-object.h>
#include <glrt/toolkit/fragmented-array.h>

namespace glrt {
namespace scene {

class TickManager : public QObject
{
  Q_OBJECT
public:
  TickManager(QObject* parent=nullptr);

  void tick(float deltaTime);

private:
  friend class TickingObject;

  struct HandlerBase
  {
    typedef TickManager* extra_data_type;
  };

  struct TickCaller : public HandlerBase
  {
    static void handle_value(const TickingObject**, int, TickManager*)
    {
    }

    static bool valueLessThan(const TickingObject* a, const TickingObject* b)
    {
      return a < b;
    }
  };

  struct DependencyDepthHandler : public HandlerBase
  {
    static bool segmentLessThan(const TickingObject* a, const TickingObject* b)
    {
      return classify(a) < classify(b);
    }

    static int classify(const TickingObject* tickingObject)
    {
      return tickingObject->tickDependencyDepth();
    }

    static int segment_as_index(int i)
    {
      return i;
    }

    static int segment_from_index(int i)
    {
      return i;
    }

    static void handle_new_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager)
    {
      Q_UNUSED(objects);
      Q_UNUSED(begin);
      Q_UNUSED(end);
      Q_UNUSED(depth);
      Q_UNUSED(tickManager);
    }

    static void handle_end_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager)
    {
      Q_UNUSED(objects);
      Q_UNUSED(begin);
      Q_UNUSED(end);
      Q_UNUSED(depth);
      Q_UNUSED(tickManager);
    }
  };

  struct MultithreadingHandler : public HandlerBase
  {
    static bool segmentLessThan(const TickingObject* a, const TickingObject* b)
    {
      return classify(a) < classify(b);
    }

    static TickingObject::TickTraits classify(const TickingObject* tickingObject)
    {
      return tickingObject->tickTraits();
    }

    static int segment_as_index(TickingObject::TickTraits i)
    {
      return static_cast<int>(i);
    }

    static TickingObject::TickTraits segment_from_index(int i)
    {
      return static_cast<TickingObject::TickTraits>(i);
    }

    static void handle_new_segment(TickingObject** objects, int begin, int end, TickingObject::TickTraits tickTraits, TickManager* tickManager)
    {
      switch(tickTraits)
      {
      case TickingObject::TickTraits::Multithreaded:
        // #ISSUE-61 OMP
        for(int i=begin; i<end; ++i)
          if(!tickManager->deletedObjects.contains(objects[i]))
            objects[i]->tick(tickManager->deltaTime);
        break;
      case TickingObject::TickTraits::OnlyMainThread:
        for(int i=begin; i<end; ++i)
          if(!tickManager->deletedObjects.contains(objects[i]))
            objects[i]->tick(tickManager->deltaTime);
        break;
      default:
        Q_UNREACHABLE();
      }
    }

    static void handle_end_segment(TickingObject** objects, int begin, int end, int depth, TickManager* tickManager)
    {
      Q_UNUSED(objects);
      Q_UNUSED(begin);
      Q_UNUSED(end);
      Q_UNUSED(depth);
      Q_UNUSED(tickManager);
    }
  };

  typedef FragmentedArray_Segment_Values<TickingObject*, TickCaller>  CallTickTrait;
  typedef FragmentedArray_Segment_Split_in_TwoSegments<TickingObject*, TickingObject::TickTraits, MultithreadingHandler, CallTickTrait> MultithreadingTraits;
  typedef FragmentedArray_Segment_Split_in_VariableNumber<TickingObject*, int, DependencyDepthHandler, MultithreadingTraits> DependencyDepthTraits;

  typedef FragmentedArray<TickingObject*, DependencyDepthTraits> FragmentedArray_TickOrder;

  FragmentedArray_TickOrder fragmented_array;
  QSet<QPointer<TickingObject>> notYetAddedTickingPointers;
  QSet<TickingObject*> deletedObjects;
  QMutex mutex;
  float deltaTime;

  void addTickingObject(TickingObject* tickingObject);
  void updateObjects();

private slots:
  void removeObject(QObject* object);
  void updateObject(TickingObject* object);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_TICKMANAGER_H
