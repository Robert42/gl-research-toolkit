#include <glrt/glsl/math-cpp.h>

#include <glrt/scene/scene-data.h>
#include <glrt/scene/resources/voxelizer.h>

#include <glrt/dependencies.h>

namespace glrt {
namespace renderer {

using scene::resources::VoxelGridGeometry;
using scene::resources::BoundingSphere;

bool sphere_intersects_convex_shape(const glsl::Plane* planes, int num_planes, const glsl::Sphere& sphere)
{
  for(int i=0; i<num_planes; ++i)
  {
    const glsl::Plane& plane = planes[i];

    if(!glsl::intersects_or_backfaced(sphere, plane))
      return false;
  }
  return true;
}

#define SPONZA_ARCH_PAD_EDGE_001 0

#define DEBUG_SINGLE_SDF SPONZA_ARCH_PAD_EDGE_001
#if DEBUG_SINGLE_SDF
extern bool debug_now;
bool debug_now = false;
#endif

Array<uint16_t> collectAllSdfIntersectingWith_ConvexShape(const glsl::Plane* planes, int num_planes, const scene::Scene::Data* data, float influence_radius)
{
  Array<uint16_t> target;

  uint16_t n = data->voxelGrids->length;

//#define FIND_SDF
#ifdef FIND_SDF
  uint16_t sdf_candidate_index = 0xffff;
  float distance_to_sdf_candidate = INFINITY;
  glm::vec3 best_position(-12.86266, 5.75949, 1.87159);
#endif

#if SPONZA_ARCH_PAD_EDGE_001
  uint16_t sponza_arch_pad_edge_001 = 93; // sponza-arch-pad-edge-001
#endif

  // TODO how to handle static and dynamic objects?
  for(uint16_t i=0; i<n; ++i)
  {
    const BoundingSphere bounding_sphere = data->voxelGrids->globalCoordFrame(i) * data->voxelGrids->boundingSphere[i];
    glsl::Sphere sphere;
    sphere.origin = bounding_sphere.center;
    sphere.radius = bounding_sphere.radius + influence_radius;

#if DEBUG_SINGLE_SDF
    if(debug_now && sponza_arch_pad_edge_001 == i)
    {
      PRINT_VALUE(sphere.origin);
      PRINT_VALUE(sphere.radius);

      for(int j=0; j<num_planes; ++j)
      {
        const glsl::Plane& plane = planes[j];

        PRINT_VALUE(plane.normal);
        PRINT_VALUE(plane.d * plane.normal);
      }
      PRINT_VALUE(sphere_intersects_convex_shape(planes, num_planes, sphere));
    }
#endif

#ifdef FIND_SDF
    if(glm::distance(bounding_sphere.center, best_position) < distance_to_sdf_candidate)
    {
      sdf_candidate_index = i;
      distance_to_sdf_candidate = glm::distance(bounding_sphere.center, best_position);
    }
#endif

    if(sphere_intersects_convex_shape(planes, num_planes, sphere))
    {
      target.append_copy(i);
    }
  }

#ifdef FIND_SDF
  PRINT_VALUE(sdf_candidate_index);
#endif

  return target;
}

Array<uint16_t> collectAllSdfIntersectingWith(const scene::Scene::Data* data, const glm::uvec3 voxel, const VoxelGridGeometry& geometry, float influence_radius)
{
#if DEBUG_SINGLE_SDF
  debug_now = false;
#if SPONZA_ARCH_PAD_EDGE_001
  debug_now = (voxel == glm::uvec3(2, 10, 6));
#endif
#endif

  const scene::CoordFrame coordFrame = geometry.toVoxelSpace.inverse();
  auto transform = [&](const glm::vec3& position){return coordFrame * position;};

  const glm::vec3 min_pos = transform(glm::vec3(voxel));
  const glm::vec3 max_pos = transform(glm::vec3(voxel+uint32_t(1)));

  glsl::Plane planes[6];

  for(int i=0; i<3; ++i)
  {
    glm::vec3 normal(0);
    normal[i] = 1;

    planes[i*2+0] = glsl::plane_from_normal( normal, max_pos);
    planes[i*2+1] = glsl::plane_from_normal(-normal, min_pos);
  }

  return collectAllSdfIntersectingWith_ConvexShape(planes, 6, data, influence_radius);
}

} // namespace renderer
} // namespace glrt
