#include <extensions/command_list.glsl>

#include <glrt/glsl/math.h>
#include <scene/uniforms.glsl>
#include <debugging/normal.glsl>

#include <lighting/rendering-equation.glsl>

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

void rayMarch(in Ray ray, inout vec4 color, out vec3 world_pos, out vec3 world_normal);

out vec4 fragment_color;
void main()
{
  Ray ray;
  ray.origin = scene.camera_position;
  ray.direction = normalize(fragment.look_target - scene.camera_position);
  
  vec3 world_pos, world_normal;
  
  fragment_color = vec4(1, 0.5, 0, 1);
  
  rayMarch(ray, fragment_color,  world_pos, world_normal);
  
#ifdef POSTEFFECT_VISUALIZATION_SHADER_SHOW_NORMALS
  fragment_color.rgb = encode_signed_normalized_vector_as_color(world_normal);
#endif
#ifdef POSTEFFECT_VISUALIZATION_SHADER_LIGHTED
  BaseMaterial material;
  material.normal = world_normal;
  material.normal_length = 1.f;
  material.base_color = fragment_color.rgb;
  material.metal_mask = 0.f;
  material.emission = vec3(0.f);
  material.reflectance = 0.5f;
  material.occlusion = 1.f;
  material.smoothness = 0.5f;
  
  vec3 incoming_luminance = light_material(material, world_pos, scene.camera_position);
  fragment_color.rgb = accurateLinearToSRGB(incoming_luminance);;
#endif
  
  vec4 point = vec4(world_pos, 1);
  
  point = fragment.view_projection * point;
  point /= point.w;
  
  gl_FragDepth = FragCoord_z_toFragDepth(point.z);
}
