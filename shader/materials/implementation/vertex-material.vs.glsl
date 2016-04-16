#define USE_PER_VERTEX_MATERIAL !defined(DEPTH_PREPASS) && (defined(PLAIN_COLOR) || defined(AREA_LIGHT))

#if USE_PER_VERTEX_MATERIAL

#include <pbs/pbs.glsl> // baseMaterial & luminance

out flat BaseMaterial plainColorMaterial;

#if defined(PLAIN_COLOR)


layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, std140) uniform MaterialInstanceBlock
{
  vec3 base_color;
  float smoothness;
  vec3 emission;
  float metal_mask;
}material_instance;

void apply_material()
{
  BaseMaterial material;
  
  material.smoothness = material_instance.smoothness;
  material.base_color = material_instance.base_color;
  material.metal_mask  = material_instance.metal_mask;
  material.emission = material_instance.emission;
  
  material.normal = normalize(fragment.normal);
  material.reflectance = 0.5f;
  material.occlusion = 1;
  
  plainColorMaterial = material;
}

#elif defined(AREA_LIGHT)

#if defined(SPHERE_LIGHT)
vec3 current_lightsource_luminance()
{
  uint32_t n;
  SphereAreaLight* sphere_lights;
  get_sphere_lights(n, sphere_lights);
  
  float radius = sphere_lights[gl_InstanceID].radius;
  float luminous_power = sphere_lights[gl_InstanceID].light.luminous_power;
  vec3 color = sphere_lights[gl_InstanceID].light.color;
  
  return luminance_for_sphere(luminous_power, radius) * color;
}
#elif defined(RECT_LIGHT)
vec3 current_lightsource_luminance()
{
  uint32_t n;
  RectAreaLight* rect_lights;
  get_rect_lights(n, rect_lights);
  
  float half_width = rect_lights[gl_InstanceID].half_width;
  float half_height = rect_lights[gl_InstanceID].half_height;
  float luminous_power = rect_lights[gl_InstanceID].light.luminous_power;
  vec3 color = rect_lights[gl_InstanceID].light.color;
  
  return luminance_for_rect(luminous_power, half_width*2.f, half_height*2.f) * color;
}
#endif



void apply_material()
{
  BaseMaterial material;
  
  material.emission = current_lightsource_luminance();
  
  material.normal = normalize(fragment.normal);
  material.smoothness = 0;
  material.base_color = vec3(0);
  material.metal_mask  = 0;
  material.reflectance = 0;
  material.occlusion = 1;
  
  plainColorMaterial = material;
}

#endif

#else // USE_PER_VERTEX_MATERIAL

void apply_material()
{
}

#endif // USE_PER_VERTEX_MATERIAL