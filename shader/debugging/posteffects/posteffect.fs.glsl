#include <extensions/command_list.glsl>

#include <glrt/glsl/math.h>
#include <scene/uniforms.glsl>


in FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;
}fragment;

float FragCoord_z_toFragDepth(float z)
{
  // TODO check for performance improvement by leaving out gl_DepthRange.diff and gl_DepthRange.near
  return (0.5 * z + 0.5) * gl_DepthRange.diff + gl_DepthRange.near;
}

vec3 rayMarch(in Ray ray, out vec4 color);

out vec4 fragment_color;
void main()
{
  Ray ray;
  ray.origin = scene.camera_position;
  ray.direction = normalize(fragment.look_target - scene.camera_position);
  
  vec4 point = vec4(rayMarch(ray, fragment_color), 1);
  
  point = fragment.view_projection * point;
  point /= point.w;
  
  gl_FragDepth = FragCoord_z_toFragDepth(point.z);
}