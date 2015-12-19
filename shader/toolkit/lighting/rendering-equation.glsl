#include <lighting/light-buffers.glsl>

#include <debugging/print.glsl>

#include <pbs/pbs.glsl>

struct LightData
{
  LightSource lightSource;
  vec3 direction_to_light;
  float illuminance;
  float specularEnergyFactor;
};

vec3 getDirectionToLight(out float specularEnergyFactor, in Disk disk, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  specularEnergyFactor = 1.f;
  return normalize(disk.origin-surface.position);
}

vec3 getDirectionToLight(out float specularEnergyFactor, in Rect rect, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  vec3 nearest_point;
  Ray ray;
  ray.origin = surface.position;
  ray.direction = get_mrp_reflection_direction(surface);
  nearest_point = mrp(rect, ray) - surface.position;
  
  PRINT_VALUE(nearest_point+surface.position, true);
  PRINT_VALUE(ray, true);
  
  float light_distance = length(nearest_point);
  float radius = mix(rect.half_width, rect.half_height, 0.5f);
  
  vec3 l = nearest_point / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(radius, light_distance, surface);
  
  
  return l;
}


vec3 do_the_lighting(in LightData light, in BrdfData_Generic brdf_g, in SurfaceData surface)
{
  vec3 L = light.direction_to_light;
  vec3 V = surface.direction_to_camera;
  vec3 N = surface.normal;
  
  BrdfData_WithLight brdf_l = init_brdf_data_with_light(N, L, V, light.specularEnergyFactor);
  
  // TODO is luminance really the right name?
  vec3 luminance = light.illuminance * light.lightSource.luminance * light.lightSource.color;
  vec3 brdf = evaluate_brdf_for_material(brdf_g, brdf_l, surface);
  float cos_factor = brdf_l.NdotL;
  
  return luminance * brdf * cos_factor;
}


vec3 rendering_equation(in BrdfData_Generic brdf_g, in SurfaceData surface)
{
  vec3 worldNormal = surface.normal;
  vec3 worldPosition = surface.position;
  vec3 viewDir = surface.direction_to_camera;
  vec3 R = reflect(-viewDir, worldNormal);
  
  vec3 outgoing_light = vec3(0);
  
  for(int i=0; i<sphere_arealights.num; ++i)
  {
    SphereAreaLight light = sphere_arealights.lights[i];
    
    Sphere sphere;
    sphere.origin = light.origin;
    sphere.radius = light.radius;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.illuminance = sphereLightIlluminance(worldNormal, worldPosition, sphere);
    light_data.direction_to_light = getDirectionToLight(light_data.specularEnergyFactor, sphere, surface);
    
    outgoing_light += do_the_lighting(light_data, brdf_g, surface);
  }
  
  for(int i=0; i<rect_arealights.num; ++i)
  {
    RectAreaLight light = rect_arealights.lights[i];
    
    Rect rect;
    rect.origin = light.origin;
    rect.tangent1 = light.tangent1;
    rect.tangent2 = light.tangent2;
    rect.half_width = light.half_width;
    rect.half_height = light.half_height;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.illuminance = rectAreaLightIlluminance(worldPosition, worldNormal, rect);
    light_data.direction_to_light = getDirectionToLight(light_data.specularEnergyFactor, rect, surface);
    
    // TODO: muoltiply instead of if?
    outgoing_light += do_the_lighting(light_data, brdf_g, surface);
  }
  
  return outgoing_light + surface.emission;
}


vec3 light_material(in BaseMaterial material, in vec3 surface_position, in vec3 camera_position)
{
  BrdfData_Generic brdf_g;
  SurfaceData surface;
  
  precomputeData(material, surface_position, camera_position, brdf_g, surface);
  
  return rendering_equation(brdf_g, surface);
}
