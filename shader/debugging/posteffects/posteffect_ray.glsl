#include <extensions/command_list.glsl>

#include <glrt/glsl/math-glsl.h>

#include <scene/uniforms.glsl>

#include <debugging/normal.glsl>
#include <write-fragment-color.glsl>
#include <write-fragment-depth.glsl>

#include <lighting/rendering-equation.glsl>

in FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;
}fragment;


Ray view_ray()
{
  Ray ray;
  ray.origin = scene.camera_position;
  ray.direction = normalize(fragment.look_target - scene.camera_position);
  return ray;
}
