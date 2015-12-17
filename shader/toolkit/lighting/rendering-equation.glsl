#include <lighting/light-buffers.glsl>

#include <debugging/print.glsl>

#include <pbs/pbs.glsl>

struct SurfaceData
{
  vec3 position;
};

struct ViewerData
{
  vec3 direction_to_camera;
};

struct LightData
{
  LightSource lightSource;
  vec3 direction_to_light;
  float illuminance;
};


vec3 do_the_lighting(in BaseMaterial material, in ViewerData viewer, in LightData light, in PrecomputedMaterial precomputedMaterial)
{
  vec3 V = viewer.direction_to_camera;
  vec3 L = light.direction_to_light;
  vec3 N = material.normal;
  
  // TODO is luminance really the right name?
  vec3 luminance = light.illuminance * light.lightSource.luminance * light.lightSource.color;
  vec3 brdf = material_brdf(V, L, material, precomputedMaterial);
  float cos_factor = max(0, dot(L, N));
  
  return luminance * brdf * cos_factor;
}


vec3 rendering_equation(in BaseMaterial material, in SurfaceData surface, in ViewerData viewer)
{
  vec3 worldNormal = material.normal;
  vec3 worldPosition = surface.position;
  vec3 viewDir = viewer.direction_to_camera;
  vec3 reflectionDir = reflect(-viewDir, worldNormal);
  
  vec3 outgoing_light = vec3(0);
  
  for(int i=0; i<sphere_arealights.num; ++i)
  {
    SphereAreaLight light = sphere_arealights.lights[i];
    
    Sphere sphere;
    sphere.origin = light.origin;
    sphere.radius = light.radius;
    
    LightData light_data;
    
    light_data.lightSource = light.light;
    light_data.direction_to_light = getDirectionToLight(sphere, worldPosition);
    light_data.illuminance = sphereLightIlluminance(worldNormal, worldPosition, sphere);
    
    outgoing_light += do_the_lighting(material, viewer, light_data);
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
    light_data.direction_to_light = getDirectionToLight(rect, worldPosition);
    light_data.illuminance = rectAreaLightIlluminance(worldPosition, worldNormal, rect);
    
    outgoing_light += do_the_lighting(material, viewer, light_data);
  }
  
  return outgoing_light + material.emission;
}


vec3 light_material(in BaseMaterial material, in SurfaceData surface, in ViewerData viewer)
{
  return rendering_equation(material, surface, viewer);
}
