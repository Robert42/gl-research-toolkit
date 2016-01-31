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
  STATIC, DYNAMIC
};


struct DummyMeshComponent
{
  bool movable;
  Material material;
  Mesh mesh;
  int instanceId;

  bool operator==(const DummyMeshComponent& other) const
  {
    return this->instanceId == other.instanceId;
  }
};

struct DummyLightComponent
{
  Interactivity interactivity;
  int instanceId;

  bool operator==(const DummyLightComponent& other) const
  {
    return this->instanceId == other.instanceId;
  }
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

  static QString format(Interactivity interactivity)
  {
    switch(interactivity)
    {
    case Interactivity::DYNAMIC:
      return "Dynamic";
    case Interactivity::STATIC:
      return "Static";
    default:
      Q_UNREACHABLE();
    }
  }

  static QString format(Mesh mesh)
  {
    switch(mesh)
    {
    case Mesh::a:
      return "a";
    case Mesh::b:
      return "b";
    case Mesh::c:
      return "c";
    case Mesh::d:
      return "d";
    case Mesh::e:
      return "e";
    case Mesh::f:
      return "f";
    default:
      Q_UNREACHABLE();
    }
  }


  static QString format(Material material)
  {
    switch(material)
    {
    case Material::A:
      return "A";
    case Material::B:
      return "B";
    case Material::C:
      return "C";
    case Material::D:
      return "D";
    case Material::E:
      return "E";
    case Material::F:
      return "F";
    default:
      Q_UNREACHABLE();
    }
  }

  static QString format(bool movable)
  {
    if(movable)
      return "Movable";
    else
      return "Static";
  }


  template<typename T, int depth=0>
  static QString format(T x, int begin, int end)
  {
    return QString("%0(%1, %2)").arg(format(x)).arg(begin).arg(end);
  }
};

struct DummyMeshCompnentHandler : public BaseHandler
{
  static void handle_value(const DummyMeshComponent* components, int index, QString* output)
  {
    *output += QString("      %0\n").arg(components[index].instanceId);
  }

  static bool valueLessThan(const DummyMeshComponent& a, const DummyMeshComponent& b)
  {
    return a.instanceId < b.instanceId;
  }
};

struct DummyLightComponentHandler : public BaseHandler
{
  static void handle_value(const DummyLightComponent* components, int index, QString* output)
  {
    *output += QString("  %0\n").arg(components[index].instanceId);
  }

  static bool valueLessThan(const DummyLightComponent& a, const DummyLightComponent& b)
  {
    return a.instanceId < b.instanceId;
  }
};

struct InteractivitySegmentHandler : public BaseHandler
{
  static void handle_new_segment(const DummyLightComponent* components, int begin, int end, Interactivity interactivity, QString* output)
  {
    *output += format(interactivity, begin, end) + "{\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static void handle_end_segment(const DummyLightComponent* components, int begin, int end, Interactivity interactivity, QString* output)
  {
    *output += "} // "+format(interactivity, begin, end) + "\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static bool segmentLessThan(const DummyLightComponent& a, const DummyLightComponent& b)
  {
    return a.interactivity < b.interactivity;
  }

  static Interactivity classify(const DummyLightComponent& data)
  {
    return data.interactivity;
  }

  static int segment_as_index(Interactivity interactivity)
  {
    return static_cast<int>(interactivity);
  }

  static Interactivity segment_from_index(int interactivity)
  {
    return static_cast<Interactivity>(glm::clamp<int>(interactivity, 0, segment_as_index(Interactivity::DYNAMIC)));
  }
};

struct MeshSegmentHandler : public BaseHandler
{
  static void handle_new_segment(const DummyMeshComponent* components, int begin, int end, Mesh mesh, QString* output)
  {
    *output += "    Mesh " + format(mesh, begin, end) + "{\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }
  static void handle_end_segment(const DummyMeshComponent* components, int begin, int end, Mesh mesh, QString* output)
  {
    *output += "    } // Mesh "+format(mesh, begin, end) + "\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static bool segmentLessThan(const DummyMeshComponent& a, const DummyMeshComponent& b)
  {
    return a.mesh < b.mesh;
  }

  static Mesh classify(const DummyMeshComponent& data)
  {
    return data.mesh;
  }

  static int segment_as_index(Mesh mesh)
  {
    return static_cast<int>(mesh);
  }

  static Mesh segment_from_index(int mesh)
  {
    return static_cast<Mesh>(glm::clamp<int>(mesh, 0, segment_as_index(Mesh::f)));
  }
};

struct MaterialSegmentHandler : public BaseHandler
{
  static void handle_new_segment(const DummyMeshComponent* components, int begin, int end, Material material, QString* output)
  {
    *output += "  Material " + format(material, begin, end) + "{\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static void handle_end_segment(const DummyMeshComponent* components, int begin, int end, Material material, QString* output)
  {
    *output += "  } // Material "+format(material, begin, end) + "\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static bool segmentLessThan(const DummyMeshComponent& a, const DummyMeshComponent& b)
  {
    return a.material < b.material;
  }

  static Material classify(const DummyMeshComponent& data)
  {
    return data.material;
  }

  static int segment_as_index(Material material)
  {
    return static_cast<int>(material);
  }

  static Material segment_from_index(int material)
  {
    return static_cast<Material>(glm::clamp<int>(material, 0, segment_as_index(Material::F)));
  }
};

struct MovableSegmentHandler : public BaseHandler
{
  static void handle_new_segment(const DummyMeshComponent* components, int begin, int end, bool movable, QString* output)
  {
    *output += format(movable, begin, end) + "{\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static void handle_end_segment(const DummyMeshComponent* components, int begin, int end, bool movable, QString* output)
  {
    *output += "} // "+format(movable, begin, end) + "\n";
    Q_UNUSED(components);
    Q_UNUSED(output);
  }

  static bool segmentLessThan(const DummyMeshComponent& a, const DummyMeshComponent& b)
  {
    return int(a.movable) < int(b.movable);
  }

  static bool classify(const DummyMeshComponent& data)
  {
    return data.movable;
  }

  static int segment_as_index(bool movable)
  {
    return static_cast<int>(movable);
  }

  static int segment_from_index(int movable)
  {
    return movable != 0;
  }
};


typedef FragmentedArray_Segment_Values<DummyMeshComponent, DummyMeshCompnentHandler> DummyMeshComponent_Trait;
typedef FragmentedArray_Segment_Values<DummyLightComponent, DummyLightComponentHandler> DummyLightComponentHandler_Trait;


namespace GenericSectionTraits {


typedef FragmentedArray_Segment_Generic<DummyMeshComponent, Mesh, MeshSegmentHandler, DummyMeshComponent_Trait> MeshHandler;
typedef FragmentedArray_Segment_Generic<DummyMeshComponent, Material, MaterialSegmentHandler, MeshHandler> MaterialHandler;
typedef FragmentedArray_Segment_Generic<DummyMeshComponent, bool, MovableSegmentHandler, MaterialHandler> MovableHandler;

typedef FragmentedArray_Segment_Generic<DummyLightComponent, Interactivity, InteractivitySegmentHandler, DummyLightComponentHandler_Trait> LightInteractivityHandler;

} // namespace GenericSectionTraits

template<typename FragmentedArray_MeshComponents, typename FragmentedArray_LightComponents>
struct FragmentedArrayTest
{

static void test_FragmentedArray_Segment()
{
  QString output;

  DummyLightComponent lightComponent1 = {Interactivity::DYNAMIC,
                                         1};
  DummyLightComponent lightComponent2 = {Interactivity::STATIC,
                                         2};
  DummyLightComponent lightComponent3 = {Interactivity::STATIC,
                                         3};
  DummyLightComponent lightComponent4 = {Interactivity::DYNAMIC,
                                         4};
  DummyLightComponent lightComponent5 = {Interactivity::STATIC,
                                         5};

  FragmentedArray_LightComponents lightComponents;
  lightComponents.append_copy(lightComponent5);
  lightComponents.append_copy(lightComponent3);
  lightComponents.append_copy(lightComponent4);
  lightComponents.append_copy(lightComponent1);
  lightComponents.append_copy(lightComponent2);

  output = "\n";
  lightComponents.updateSegments(&output);

  output = "\n";
  lightComponents.iterate(&output);
  EXPECT_EQ(output,
            "\n"
            "Static(0, 3){\n"
            "  2\n"
            "  3\n"
            "  5\n"
            "} // Static(0, 3)\n"
            "Dynamic(3, 5){\n"
            "  1\n"
            "  4\n"
            "} // Dynamic(3, 5)\n");
}


static void test_FragmentedArray_Segment_recursive()
{
  QString output;
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

  FragmentedArray_MeshComponents meshComponents;
  meshComponents.append_copy(meshComponent5);
  meshComponents.append_copy(meshComponent3);
  meshComponents.append_copy(meshComponent1);
  meshComponents.append_copy(meshComponent4);
  meshComponents.append_copy(meshComponent2);

  output = "\n";
  meshComponents.updateSegments(&output);

  output = "\n";
  meshComponents.iterate(&output);
  EXPECT_EQ(output,
            "\n"
            "Static(0, 1){\n"
            "  Material A(0, 1){\n"
            "    Mesh a(0, 1){\n"
            "      2\n"
            "    } // Mesh a(0, 1)\n"
            "  } // Material A(0, 1)\n"
            "} // Static(0, 1)\n"
            "Movable(1, 5){\n"
            "  Material A(1, 3){\n"
            "    Mesh a(1, 3){\n"
            "      1\n"
            "      3\n"
            "    } // Mesh a(1, 3)\n"
            "  } // Material A(1, 3)\n"
            "  Material B(3, 4){\n"
            "    Mesh c(3, 4){\n"
            "      5\n"
            "    } // Mesh c(3, 4)\n"
            "  } // Material B(3, 4)\n"
            "  Material D(4, 5){\n"
            "    Mesh a(4, 5){\n"
            "      4\n"
            "    } // Mesh a(4, 5)\n"
            "  } // Material D(4, 5)\n"
            "} // Movable(1, 5)\n");
}

static void test_FragmentedArray_Segment_recursive_updating_only_the_last_segmet()
{
  QString output;
  int region_to_be_updated;

  DummyLightComponent lightComponent1 = {Interactivity::DYNAMIC,
                                         1};
  DummyLightComponent lightComponent2 = {Interactivity::STATIC,
                                         2};
  DummyLightComponent lightComponent3 = {Interactivity::STATIC,
                                         3};
  DummyLightComponent lightComponent4 = {Interactivity::DYNAMIC,
                                         4};
  DummyLightComponent lightComponent5 = {Interactivity::STATIC,
                                         5};

  FragmentedArray_LightComponents lightComponents;
  lightComponents.append_copy(lightComponent5);
  lightComponents.append_copy(lightComponent3);
  lightComponents.append_copy(lightComponent4);
  lightComponents.append_copy(lightComponent1);
  lightComponents.append_copy(lightComponent2);

  output = "\n";
  region_to_be_updated = lightComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 0);

  lightComponents.append_copy(lightComponent1);

  output = "\n";
  region_to_be_updated = lightComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 3);

  lightComponents.append_copy(lightComponent1);

  output = "\n";
  region_to_be_updated = lightComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 3);

  lightComponents.append_copy(lightComponent4);

  output = "\n";
  region_to_be_updated = lightComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 3);

  lightComponents.append_copy(lightComponent3);

  output = "\n";
  region_to_be_updated = lightComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 0);

  lightComponents.append_copy(lightComponent2);

  output = "\n";
  region_to_be_updated = lightComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 0);


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

  FragmentedArray_MeshComponents meshComponents;
  meshComponents.append_copy(meshComponent5);
  meshComponents.append_copy(meshComponent3);
  meshComponents.append_copy(meshComponent1);
  meshComponents.append_copy(meshComponent4);
  meshComponents.append_copy(meshComponent2);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 0);

  meshComponents.append_copy(meshComponent4);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 4);

  meshComponents.append_copy(meshComponent4);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 4);

  meshComponents.append_copy(meshComponent4);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 4);

  meshComponents.append_copy(meshComponent5);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 3);

  meshComponents.append_copy(meshComponent5);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 3);

  meshComponents.remove(meshComponent5);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 3);

  meshComponents.append_copy(meshComponent3);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 1);

  meshComponents.append_copy(meshComponent3);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 1);

  meshComponents.remove(meshComponent3);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 1);

  meshComponents.remove(meshComponent3);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 1);

  meshComponents.remove(meshComponent3);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 1);

  meshComponents.append_copy(meshComponent2);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 0);

  meshComponents.remove(meshComponent2);

  output = "\n";
  region_to_be_updated = meshComponents.updateSegments(&output);
  EXPECT_EQ(region_to_be_updated, 0);
}

static void test_fragmented_array()
{
  test_FragmentedArray_Segment();
  test_FragmentedArray_Segment_recursive();
  test_FragmentedArray_Segment_recursive_updating_only_the_last_segmet();
}

};

void test_fragmented_array()
{
  typedef FragmentedArray<DummyMeshComponent, GenericSectionTraits::MovableHandler> FragmentedArray_MeshComponents_GenericOnly;
  typedef FragmentedArray<DummyLightComponent, GenericSectionTraits::LightInteractivityHandler> FragmentedArray_LightComponents_GenericOnly;
  FragmentedArrayTest<FragmentedArray_MeshComponents_GenericOnly, FragmentedArray_LightComponents_GenericOnly>::test_fragmented_array();
}
