#include <glrt/scene/scene-data.h>
#include <glrt/scene/resources/voxelizer.h>

#include <glrt/glsl/math-cpp.h>

namespace glrt {
namespace renderer {

using scene::resources::VoxelGridGeometry;
using scene::resources::BoundingSphere;

bool sphere_intersects_convex_shape(const glsl::Plane* planes, int num_planes, const glsl::Sphere& sphere)
{
  for(int i=0; i<num_planes; ++i)
  {
    const glsl::Plane& plane = planes[i];

    if(glsl::intersects_or_backfaced(sphere, plane))
      return true;
  }
  return false;
}

Array<uint16_t> collectAllSdfIntersectingWith_ConvexShape(const glsl::Plane* planes, int num_planes, const scene::Scene::Data* data, float influence_radius)
{
  Array<uint16_t> target;
  const BoundingSphere* boundingSpheres = data->voxelGrids->boundingSphere;

  uint16_t n = data->voxelGrids->length;

  // TODO how to handle static and dynamic objects?
  for(uint16_t i=0; i<n; ++i)
  {
    glsl::Sphere sphere;
    sphere.origin = boundingSpheres[i].center;
    sphere.radius = boundingSpheres[i].radius + influence_radius;

    if(sphere_intersects_convex_shape(planes, num_planes, sphere))
      target.append_copy(i);
  }

  return target;
}

Array<uint16_t> collectAllSdfIntersectingWith(const scene::Scene::Data* data, const glm::uvec3 voxel, const VoxelGridGeometry& geometry, float influence_radius)
{
  const glm::vec3 min_pos = geometry.toVoxelSpace * glm::vec3(voxel);
  const glm::vec3 max_pos = geometry.toVoxelSpace * glm::vec3(voxel+uint32_t(1));
  const scene::CoordFrame coordFrame = geometry.toVoxelSpace.inverse();

  glsl::Plane planes[6];

  for(int i=0; i<3; ++i)
  {
    glm::vec3 normal(0);
    normal[i] = 1;

    planes[i*2+0] = coordFrame * glsl::plane_from_normal(normal, max_pos);
    planes[i*2+1] = coordFrame * glsl::plane_from_normal(-normal, min_pos);
  }

  return collectAllSdfIntersectingWith_ConvexShape(planes, 6, data, influence_radius);
}

} // namespace renderer
} // namespace glrt
