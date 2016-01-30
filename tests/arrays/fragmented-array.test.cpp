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

struct BaseHandler
{
  typedef QString* extra_data_type;
};

struct DummyMeshCompnentHandler : public BaseHandler
{
  static void handle_value(DummyMeshComponent* components, int index)
  {
  }
};

struct DummyLightComponentHandler : public BaseHandler
{
  static void handle_value(DummyLightComponent* components, int index)
  {
  }
};

struct InteractivitySectionHandler : public BaseHandler
{
  static void handle_new_segment(DummyLightComponent* components, int begin, int end, Interactivity interactivity, QString* output)
  {
  }
};

struct MeshSectionHandler : public BaseHandler
{
  static void handle_new_segment(DummyMeshComponent* components, int begin, int end, Mesh mesh, QString* output)
  {
  }
};

struct MaterialSectionHandler : public BaseHandler
{
  static void handle_new_segment(DummyMeshComponent* components, int begin, int end, Material material, QString* output)
  {
  }
};

struct MovableSectionHandler : public BaseHandler
{
  static void handle_new_segment(DummyMeshComponent* components, int begin, int end, bool movable, QString* output)
  {
  }
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
  // #TODO::::::::::::::::::::::::::::::::::::::::::::::::::::::::.
}


void test_FragmentedArray_Segment_Generic_recursive()
{
  DummyMeshComponent meshComponent1 = {true, // movable
                                       Material::A,
                                       Mesh::a,
                                       1};
  DummyMeshComponent meshComponent2 = {false, // static
                                       Material::A,
                                       Mesh::a,
                                       2};
  DummyMeshComponent meshComponent3 = {true, // movable
                                       Material::A,
                                       Mesh::a,
                                       3};
  DummyMeshComponent meshComponent4 = {true, // movable
                                       Material::D,
                                       Mesh::a,
                                       4};
  DummyMeshComponent meshComponent5 = {true, // movable
                                       Material::B,
                                       Mesh::c,
                                       5};

  DummyLightComponent lightComponent1 = {Interactivity::DYNAMIC,
                                         1};
  DummyLightComponent lightComponent2 = {Interactivity::STATIC,
                                         2};
  DummyLightComponent lightComponent3 = {Interactivity::NO_MOVABLE_DYNAMIC,
                                         3};
  DummyLightComponent lightComponent4 = {Interactivity::STATIC,
                                         4};
  DummyLightComponent lightComponent5 = {Interactivity::NO_MOVABLE_DYNAMIC,
                                         5};

  FragmentedArray_MeshComponents_GenericOnly meshComponents;
  meshComponents.append_copy(meshComponent1);
  meshComponents.append_copy(meshComponent2);
  meshComponents.append_copy(meshComponent3);
  meshComponents.append_copy(meshComponent4);
  meshComponents.append_copy(meshComponent5);

  FragmentedArray_LightComponents_GenericOnly lightComponents;
  lightComponents.append_copy(lightComponent1);
  lightComponents.append_copy(lightComponent2);
  lightComponents.append_copy(lightComponent3);
  lightComponents.append_copy(lightComponent4);
  lightComponents.append_copy(lightComponent5);
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
