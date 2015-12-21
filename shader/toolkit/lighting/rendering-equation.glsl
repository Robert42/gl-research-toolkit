#include <lighting/light-buffers.glsl>

#include <pbs/pbs.glsl>

#include "light-direction.glsl"

struct LightData
{
  LightSource lightSource;
  vec3 direction_to_light;
  float illuminance;
  float specularEnergyFactor;
  float distance_to_light;
};

float light_falloff(in LightData light, in SurfaceData surface)
{
  float distance_for_influence = length(light.lightSource.origin-surface.position);
  float influence_radius = light.lightSource.influence_radius;
  float distance_to_light = light.distance_to_light;
  
  return light_falloff(distance_for_influence, influence_radius, distance_to_light);
}

vec3 do_the_lighting(in LightData light, in BrdfData_Generic brdf_g, in SurfaceData surface)
{
  vec3 L = light.direction_to_light;
  vec3 V = surface.direction_to_camera;
  vec3 N = surface.normal;
  
  BrdfData_WithLight brdf_l = init_brdf_data_with_light(N, L, V, light.specularEnergyFactor);
  
  vec3 illuminance = light.illuminance * light.lightSource.luminous_power * light.lightSource.color * light_falloff(light, surface);
  vec3 brdf = evaluate_brdf_for_material(brdf_g, brdf_l, surface);
  float cos_factor = brdf_l.NdotL;
  
  return illuminance * brdf * cos_factor;
}


vec3 rendering_equation(in BrdfData_Generic brdf_g, in SurfaceData surface)
{
  vec3 worldNormal = surface.normal;
  vec3 worldPosition = surface.position;
  vec3 viewDir = surface.direction_to_camera;
  vec3 R = reflect(-viewDir, worldNormal);
  
  vec3 outgoing_luminance = vec3(0);
  
  for(int i=0; i<sphere_arealights.num; ++i)
  {
    SphereAreaLight light = sphere_arealights.lights[i];
    
    Sphere sphere;
    sphere.origin = light.light.origin;
    sphere.radius = light.radius;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.illuminance = sphereLightIlluminance(worldNormal, worldPosition, sphere);
    light_data.direction_to_light = getDirectionToLight(light_data.specularEnergyFactor, light_data.distance_to_light, sphere, surface, surface.dominant_specular_dir);
    
    outgoing_luminance += do_the_lighting(light_data, brdf_g, surface);
  }
  
  for(int i=0; i<rect_arealights.num; ++i)
  {
    RectAreaLight light = rect_arealights.lights[i];
    
    Rect rect;
    rect.origin = light.light.origin;
    rect.tangent1 = light.tangent1;
    rect.tangent2 = light.tangent2;
    rect.half_width = light.half_width;
    rect.half_height = light.half_height;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.illuminance = rectAreaLightIlluminance(worldPosition, worldNormal, rect);
    light_data.direction_to_light = getDirectionToLight(light_data.specularEnergyFactor, light_data.distance_to_light, rect, surface, surface.dominant_specular_dir);
    
    outgoing_luminance += do_the_lighting(light_data, brdf_g, surface);
  }
  
  return outgoing_luminance + surface.emission;
}


vec3 light_material(in BaseMaterial material, in vec3 surface_position, in vec3 camera_position)
{
  BrdfData_Generic brdf_g;
  SurfaceData surface;
  
  precomputeData(material, surface_position, camera_position, brdf_g, surface);
  
  return rendering_equation(brdf_g, surface);
}
