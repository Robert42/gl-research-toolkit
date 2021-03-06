#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/math-glsl.h>
#include <voxels/raymarching-distance-cone-soft-shadow.glsl>
#include <voxels/ambient-occlusion-cones.glsl>
#include "light-structs.glsl"

#include <pbs/pbs.glsl>

#include "light-direction.glsl"


struct LightData
{
  LightSource lightSource;
  vec3 direction_to_light_specular;
  float distance_to_light_specular;
  vec3 direction_to_light_diffuse;
  float distance_to_light_diffuse;
  float illuminance;
  float specularEnergyFactor;
};

float light_falloff(in LightSource lightSource, in float distance_to_light, in SurfaceData surface)
{
  float distance_for_influence = length(lightSource.origin-surface.position);
  float influence_radius = lightSource.influence_radius;
  
  return light_falloff(distance_for_influence, influence_radius, distance_to_light);
}

vec3 do_the_lighting(in LightData light, in BrdfData_Generic brdf_g, in SurfaceData surface)
{
  vec3 L_specular = light.direction_to_light_specular;
  vec3 L_diffuse  = light.direction_to_light_diffuse;
  vec3 V = surface.direction_to_camera;
  vec3 N = surface.normal;
  
  BrdfData_WithLight brdf_l_diffuse = init_brdf_data_with_light(N, L_diffuse, V);
  BrdfData_WithLight brdf_l_specular = init_brdf_data_with_light(N, L_specular, V);
  
  vec3 illuminance = light.illuminance * light.lightSource.luminous_power * light.lightSource.color;
  vec3 brdf_specular = evaluate_specular_brdf_for_material(brdf_g, brdf_l_specular, surface) * light_falloff(light.lightSource, light.distance_to_light_specular, surface);
  vec3 brdf_diffuse  = evaluate_diffuse_brdf_for_material( brdf_g, brdf_l_diffuse,  surface) * light_falloff(light.lightSource, light.distance_to_light_diffuse, surface);
  float cos_factor_specular = brdf_l_specular.NdotL;
  float cos_factor_diffuse  = brdf_l_diffuse.NdotL;
  vec3 brdf = brdf_diffuse * cos_factor_diffuse + light.specularEnergyFactor * brdf_specular * cos_factor_specular;
  
  return illuminance * brdf;
}

void get_sphere_lights(out uint32_t num_sphere_lights, out SphereAreaLight* sphere_lights);
void get_rect_lights(out uint32_t num_rect_lights, out RectAreaLight* rect_lights);

vec3 rendering_equation(in BrdfData_Generic brdf_g, in SurfaceData surface)
{
  float dummy;
  vec3 worldNormal = surface.normal;
  vec3 worldPosition = surface.position;
  vec3 viewDir = surface.direction_to_camera;
  vec3 R = reflect(-viewDir, worldNormal);
  
  vec3 outgoing_luminance = vec3(0);

#ifndef NO_LIGHTING
  Sphere* bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* distance_field_data_blocks = distance_fields_voxelData();
  uint32_t num_distance_fields = distance_fields_num();
  
  uint32_t num_sphere_lights;
  SphereAreaLight* sphere_lights;

  get_sphere_lights(num_sphere_lights, sphere_lights);

  for(uint32_t i=0; i<num_sphere_lights; ++i)
  {
    SphereAreaLight light = sphere_lights[i];
    
    Sphere sphere;
    sphere.origin = light.light.origin;
    sphere.radius = light.radius;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.illuminance = sphereLightIlluminance(worldNormal, worldPosition, sphere);
    light_data.direction_to_light_specular = getDirectionToLight(light_data.specularEnergyFactor, light_data.distance_to_light_specular, sphere, surface, surface.dominant_specular_dir);
    light_data.direction_to_light_diffuse  = getDirectionToLight(dummy, light_data.distance_to_light_diffuse,  sphere, surface, surface.dominant_diffuse_dir);
    
    float occlusion = 1.f;
    
#if defined(CONETRACED_SHADOW)
    float cone_length = distance(worldPosition, sphere.origin);
    Cone cone = cone_from_point_to_sphere(worldPosition, sphere);
    cone_length *= max(0, sign(dot(worldNormal, cone.direction)));
    occlusion = coneSoftShadow(cone, bounding_spheres, distance_field_data_blocks, num_distance_fields, cone_length);
    occlusion = smooth_shadow_occlusion_value(occlusion);
    // TODO: Also other light sources should occlude this ligth source?
#endif

    outgoing_luminance += occlusion * do_the_lighting(light_data, brdf_g, surface);
  }
  
  uint32_t num_rect_lights;
  RectAreaLight* rect_lights;

  get_rect_lights(num_rect_lights, rect_lights);

  for(uint32_t i=0; i<num_rect_lights; ++i)
  {
    RectAreaLight light = rect_lights[i];
    
    Rect rect;
    rect.origin = light.light.origin;
    rect.tangent1 = light.tangent1;
    rect.tangent2 = light.tangent2;
    rect.half_width = light.half_width;
    rect.half_height = light.half_height;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.illuminance = rectAreaLightIlluminance(worldPosition, worldNormal, rect);
    light_data.direction_to_light_specular = getDirectionToLight(light_data.specularEnergyFactor, light_data.distance_to_light_specular, rect, surface, surface.dominant_specular_dir);
    light_data.direction_to_light_diffuse  = getDirectionToLight(dummy, light_data.distance_to_light_diffuse,  rect, surface, surface.dominant_diffuse_dir);
    
    outgoing_luminance += do_the_lighting(light_data, brdf_g, surface);
  }
  
#endif

#if defined(IBL_DIFFUSE)
  outgoing_luminance += evaluateIBLDiffuse(viewDir, worldNormal, R, surface.roughness, brdf_g.NdotV) * surface.diffuse_color * surface.diffuse_occlusion;
#endif
#if defined(IBL_SPECULAR)
  outgoing_luminance += evaluateIBLSpecular(surface.f0, surface.f90, worldNormal, R, surface.roughness, brdf_g.NdotV) * surface.specular_occlusion;
#endif

  return outgoing_luminance + surface.emission;
}


vec3 light_material(in BaseMaterial material, in vec3 surface_position, in vec3 camera_position)
{
  BrdfData_Generic brdf_g;
  SurfaceData surface;

  material.occlusion = 1.f
#if defined(AO_SDF)
   * distancefield_ao()
#endif
#if defined(AO_TEXTURE)
   * material.occlusion
#endif
   ;

  precomputeData(material, surface_position, camera_position, brdf_g, surface);
  
  return rendering_equation(brdf_g, surface);
}
