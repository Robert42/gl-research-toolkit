#include <glrt/renderer/voxel-buffer.h>
#include <glrt/renderer/bvh-usage.h>
#include <glrt/glsl/math-cpp.h>

namespace glrt {
namespace renderer {

using glsl::Cone;
using glm::mat4x4;
using glm::ivec3;
using glm::vec3;


void BVH::testOcclusion() const
{
  if(glrt::scene::HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE!=1 || !glrt::scene::ENFORCE_HUGE_BVH_LEAVES_FIRST || glrt::scene::SORT_OBJECTS_BY_SDF_TEXTURE)
    return;

/*
#ifdef QT_DEBUG

  glsl::Cone cone_flagpole = glsl::cone_from_ray_tan_angle(vec3(-3.497906f, -0.867900f, 0.000001f), // origin
                                                           normalize(vec3(-0.248960f, 0.209587f, 0.945565f)), // direction
                                                           0.0142003f // tan_half_angle
                                                           );

  glsl::Sphere sphere_flagpole;
  sphere_flagpole.origin = vec3(-5.58566999f, 1.59861326f, 7.78433561f);
  sphere_flagpole.radius = 0.895080328f;

//  const uint16_t roof = 52;
//  const uint16_t vase = 22;
  const uint16_t flagpole = 208;

  if(num_leaves <= flagpole || glm::distance(sphere_flagpole.origin, sphere_flagpole.origin)>1.e-5f || glm::distance(sphere_flagpole.radius, sphere_flagpole.radius)>1.e-5f)
    return; // not the sponza_full scene

  float dummy;
  const glsl::Sphere* const leaf_bounding_spheres = reinterpret_cast<const glsl::Sphere*>(this->leaves_bounding_spheres);

  Q_ASSERT(cone_intersects_sphere(cone_flagpole, sphere_flagpole, dummy));
  Q_ASSERT(cone_intersects_sphere(cone_flagpole, leaf_bounding_spheres[flagpole], dummy));
  Q_ASSERT(BVH::shadow_occlusion_without_bvh(cone_flagpole).contains(flagpole));
  Q_ASSERT(BVH::shadow_occlusion_with_bvh(cone_flagpole).contains(flagpole));
#endif
*/
}

QSet<quint16> BVH::shadow_occlusion_without_bvh(const Cone& cone) const
{
  QSet<quint16> leaves;
  float dummy;
  const glsl::Sphere* leaf_bounding_spheres = reinterpret_cast<const glsl::Sphere*>(this->leaves_bounding_spheres);

  for(uint16_t i=0; i<num_leaves; ++i)
  {
    if(cone_intersects_sphere(cone, leaf_bounding_spheres[i], dummy))
      leaves.insert(i);
  }

  return leaves;
}

QSet<quint16> BVH::shadow_occlusion_with_bvh(const Cone& cone, uint16_t root_node) const
{
  const uint16_t* inner_nodes = reinterpret_cast<const uint16_t*>(this->bvhInnerNodes);
  const glsl::Sphere* bvh_inner_bounding_sphere = reinterpret_cast<const glsl::Sphere*>(this->bvhInnerBoundingSpheres);
  const glsl::Sphere* leaf_bounding_spheres = reinterpret_cast<const glsl::Sphere*>(this->leaves_bounding_spheres);

  uint16_t stack[MAX_NUM_STATIC_MESHES];
  stack[0] = root_node;
  uint16_t stack_depth=uint16_t(1);

  QSet<quint16> leaves;
  float dummy;

  do {
    stack_depth--;
    uint16_t current_node = stack[stack_depth];

    const uint16_t* child_nodes = inner_nodes + current_node*uint16_t(2);
    uint16_t left_node = child_nodes[0];
    uint16_t right_node = child_nodes[1];

    bool left_is_inner_node = (left_node & uint16_t(0x8000)) == uint16_t(0);
    bool right_is_inner_node = (right_node & uint16_t(0x8000)) == uint16_t(0);

    left_node = left_node & uint16_t(0x7fff);
    right_node = right_node & uint16_t(0x7fff);

    if(left_is_inner_node)
    {
      if(glsl::cone_intersects_sphere(cone, bvh_inner_bounding_sphere[left_node], dummy))
        stack[stack_depth++] = left_node;
    }else
    {
      if(cone_intersects_sphere(cone, leaf_bounding_spheres[left_node], dummy))
        leaves.insert(left_node);
    }

    if(right_is_inner_node)
    {
      if(cone_intersects_sphere(cone, bvh_inner_bounding_sphere[right_node], dummy))
        stack[stack_depth++] = right_node;
    }else
    {
      if(cone_intersects_sphere(cone, leaf_bounding_spheres[right_node], dummy))
        leaves.insert(right_node);
    }

  }while(stack_depth>uint16_t(0));

  return leaves;
}

} // namespace renderer
} // namespace glrt
