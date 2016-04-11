#include <extensions/command_list.glsl>

#include <glrt/glsl/math.h>
#include <gl-wiki/frag_depth.glsl>
#include <scene/uniforms.fs.glsl>


in FragmentBlock
{
  flat mat4 view_projection;
  vec3 look_target;
}fragment;

float rayMarch(in Ray ray, out vec4 color);

out vec4 fragment_color;
void main()
{
  Ray ray;
  ray.origin = scene.camera_position;
  ray.direction = normalize(fragment.look_target - scene.camera_position);
  
  float point_distance = rayMarch(ray, fragment_color);
  
  vec4 point = vec4(get_point(ray, point_distance), 1);
  
  gl_FragDepth = FragCoord_z_toFragDepth(0.f);
}
