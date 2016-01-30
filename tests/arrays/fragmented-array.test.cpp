#include <glrt/toolkit/fragmented-array.h>

#include <testing-framework.h>

using namespace glrt;

enum class Material
{
  A,B,C,D,E,F
};

enum class Mesh
{
  a,b,c,d,e,f
};

enum class Interactivity
{
  STATIC,NO_MOVABLE_DYNAMIC,DYNAMIC
};


struct DummyMeshComponent
{
  bool movable;
  Material material;
  Mesh mesh;
  int instanceId;
};

struct DummyLightComponent
{
  Interactivity interactivity;
  int instanceId;
};





namespace glrt {

template<>
struct DefaultTraits<DummyMeshComponent>
{
  typedef ArrayTraits_Unordered_POD<DummyMeshComponent>type;
};

template<>
struct DefaultTraits<DummyLightComponent>
{
  typedef ArrayTraits_Unordered_POD<DummyLightComponent>type;
};

template<>
struct DefaultTraits<Material>
{
  typedef ArrayTraits_Unordered_Primitive<Material>type;
};

template<>
struct DefaultTraits<Mesh>
{
  typedef ArrayTraits_Unordered_Primitive<Mesh>type;
};

template<>
struct DefaultTraits<Interactivity>
{
  typedef ArrayTraits_Unordered_Primitive<Interactivity>type;
};

} // namespace glrt



struct DummyMeshCompnentHandler
{
};

struct DummyLightComponentHandler
{
};

struct InteractivitySectionHandler
{
};

struct MeshSectionHandler
{
};

struct MaterialSectionHandler
{
};

struct MovableSectionHandler
{
};


typedef FragmentedArray_Segment_Values<DummyMeshComponent, DummyMeshCompnentHandler> DummyMeshComponent_Trait;
typedef FragmentedArray_Segment_Values<DummyLightComponent, DummyLightComponentHandler> DummyLightComponentHandler_Trait;


namespace GenericSectionTraits {


typedef FragmentedArray_Segment_Generic<DummyMeshComponent, Mesh, MeshSectionHandler, DummyMeshComponent_Trait> MeshHandler;
typedef FragmentedArray_Segment_Generic<DummyMeshComponent, Material, MaterialSectionHandler, MeshHandler> MaterialHandler;
typedef FragmentedArray_Segment_Generic<DummyMeshComponent, bool, MovableSectionHandler, MaterialHandler> MovableHandler;

typedef FragmentedArray_Segment_Generic<DummyLightComponent, Interactivity, InteractivitySectionHandler, DummyLightComponentHandler_Trait> LightInteractivityHandler;

} // namespace GenericSectionTraits

typedef FragmentedArray<DummyMeshComponent, GenericSectionTraits::MovableHandler> FragmentedArray_MeshComponents_GenericOnly;
typedef FragmentedArray<DummyLightComponent, GenericSectionTraits::LightInteractivityHandler> FragmentedArray_LightComponents_GenericOnly;

void test_FragmentedArray_Segment_Generic()
{

}


void test_FragmentedArray_Segment_Generic_recursive()
{
  // #TODO::::::::::::::::::::::::::::::::::::::::::::::::::::::::.
}

void test_FragmentedArray_Segment_Generic_recursive_updating_only_the_last_segmet()
{
  // #TODO::::::::::::::::::::::::::::::::::::::::::::::::::::::::.
}

void test_fragmented_array()
{
  test_FragmentedArray_Segment_Generic();
  test_FragmentedArray_Segment_Generic_recursive();
  test_FragmentedArray_Segment_Generic_recursive_updating_only_the_last_segmet();
}
