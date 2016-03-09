#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

#include <glrt/glsl/layout-constants.h>


struct SceneLightData
{
  uint64_t sphere_arealights_address;
  uint64_t rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

layout(binding=UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK, std140) uniform SceneFragmentBlock
{
  vec3 camera_position;
  SceneLightData lights;
}scene;

out vec4 color;

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
  PRINT_VALUE(scene.camera_position);
  
#ifdef NO_LIGHTING
  color = vec4(material.base_color, 1);
  return;
#endif

  vec3 incoming_luminance = light_material(material, surface.position, scene.camera_position);
  
  float exposure = 1.f; // Only dummy value, to be corrected
  
  incoming_luminance *= exposure;
  
#ifdef DEPTH_PREPASS
  color = vec4(vec3(0), alpha);
#endif

#ifdef FORWARD_PASS
  color = vec4(accurateLinearToSRGB(incoming_luminance), alpha);
#endif
}

void get_sphere_lights(out uint32_t num_sphere_lights, out SphereAreaLight* sphere_lights)
{
  num_sphere_lights = scene.lights.num_sphere_area_lights;
  sphere_lights = (SphereAreaLight*)scene.lights.sphere_arealights_address;
}

void get_rect_lights(out uint32_t num_rect_lights, out RectAreaLight* rect_lights)
{
  num_rect_lights = scene.lights.num_rect_area_lights;
  rect_lights = (RectAreaLight*)scene.lights.rect_arealights_address;
}


#endif // FORWARD_RENDERER
