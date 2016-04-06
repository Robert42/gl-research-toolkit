#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>
#include <debugging/normal.glsl>

#include <glrt/glsl/layout-constants.h>


struct SceneLightData
{
  SphereAreaLight* sphere_arealights_address;
  RectAreaLight* rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

struct SceneFragmentData
{
  vec3 camera_position;
  SceneLightData lights;
};

layout(std140, binding=UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK) uniform SceneFragmentBlock
{
  SceneFragmentData scene;
};

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

#if defined(MESH_NORMALS_WS)
  fragment_color = vec4(encode_direction_as_color(normalize(fragment.normal)), 1);
  return;
#elif defined(MESH_UVS)
  fragment_color = vec4(fragment.uv, 0, 1);
  return;
#elif defined(MESH_TANGENTS_WS)
  fragment_color = vec4(encode_direction_as_color(normalize(fragment.tangent)), 1);
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
