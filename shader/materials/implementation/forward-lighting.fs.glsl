#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>
#include <debugging/normal.glsl>

#include <glrt/glsl/layout-constants.h>

#include <scene/uniforms.fs.glsl>

out vec4 fragment_color;

#include "highlight-numeric-issues.glsl"

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
#ifdef MASKED
  alpha = step(MASK_THRESHOLD, alpha);
#endif

#ifdef LIGHTING_FLAT
  fragment_color = vec4(material.base_color + material.emission, alpha);
  return;
#endif

#if defined(MATERIAL_NORMAL_WS)
  fragment_color = vec4(encode_direction_as_color(material.normal), alpha);
  return;
#elif defined(MATERIAL_ALPHA)
  fragment_color = vec4(vec3(alpha), 1);
  return;
#elif defined(MATERIAL_SMOOTHNESS)
  fragment_color = vec4(vec3(material.smoothness), alpha);
  return;
#elif defined(MATERIAL_BASE_COLOR)
  fragment_color = vec4(material.base_color, alpha);
  return;
#elif defined(MATERIAL_METAL_MASK)
  fragment_color = vec4(vec3(material.metal_mask), alpha);
return;
#elif defined(MATERIAL_EMISSION)
  fragment_color = vec4(material.emission, alpha);
  return;
#elif defined(MATERIAL_REFLECTANCE)
  fragment_color = vec4(vec3(material.reflectance), alpha);
  return;
#elif defined(MATERIAL_OCCLUSION)
  fragment_color = vec4(vec3(material.occlusion), alpha);
  return;
#endif

#if defined(MESH_NORMALS_WS)
  fragment_color = vec4(encode_direction_as_color(fragment.normal), 1);
  return;
#elif defined(MESH_UVS)
  fragment_color = vec4(fragment.uv, 0, 1);
  return;
#elif defined(MESH_TANGENTS_WS)
  fragment_color = vec4(encode_direction_as_color(fragment.tangent), 1);
  return;
#elif defined(MESH_BITANGENTS_WS)
  fragment_color = vec4(encode_direction_as_color(fragment.bitangent), 1);
  return;
#endif

  vec3 incoming_luminance = light_material(material, surface.position, scene.camera_position);
  
  float exposure = 1.f; // Only dummy value, to be corrected
  
  incoming_luminance *= exposure;

  fragment_color = vec4(accurateLinearToSRGB(incoming_luminance), alpha);

  highlight_numeric_issues();
}

void get_sphere_lights(out uint32_t num_sphere_lights, out SphereAreaLight* sphere_lights)
{
  num_sphere_lights = scene.lights.num_sphere_area_lights;
  sphere_lights = scene.lights.sphere_arealights_address;
}

void get_rect_lights(out uint32_t num_rect_lights, out RectAreaLight* rect_lights)
{
  num_rect_lights = scene.lights.num_rect_area_lights;
  rect_lights = scene.lights.rect_arealights_address;
}


#endif // FORWARD_RENDERER
