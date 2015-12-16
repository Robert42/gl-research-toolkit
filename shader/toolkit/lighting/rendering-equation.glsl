#include <lighting/light-buffers.glsl>

#include <debugging/print.glsl>

#include <pbs/pbs.glsl>

struct MaterialInput
{
  DisneyBaseMaterial disneyBaseMaterial;
  vec3 emission;
  vec3 position;
};

struct ViewerInput
{
  vec3 directionToCamera;
};

struct LightInput
{
  LightSource lightSource;
  vec3 direction_to_light;
  float illuminance;
};

vec3 brdf(in ShadingInput shading_input)
{
  // TODO: integrate better and performanter (don't do all steps for every light)
  
  DisneyBaseMaterial material;
  
  material.normal = shading_input.surface_normal;
  material. = shading_input.surface_normal;
  
  vec3 V = shading_input.direction_to_viewer;
  vec3 L = shading_input.direction_to_light;
  
  vec3 diffuse_term = lambertian_brdf(shading_input);
  //vec3 specular_term = blinn_phong_brdf(shading_input);
  vec3 specular_term = test_sharp_highlight(shading_input);
  
  // Just adding them is ok, because of the invariant (diffuse_color + specular_color) <= 1
  return diffuse_term + specular_term;
}

vec3 do_the_lighting(in MaterialInput material, in ViewerInput viewer, in LightInput light)
{
  vec3 direction_to_light = shading_input.direction_to_light;
  vec3 luminance = light.illuminance * light.luminance * light.color;
  float cos_factor = max(0, dot(direction_to_light, shading_input.surface_normal));
  
  return luminance * brdf(shading_input) * cos_factor;
}


vec3 rendering_equation(in MaterialInput material, in ViewerInput viewer)
{
  vec3 worldNormal = material.material.normal;
  vec3 worldPosition = material.position;
  
  vec3 outgoing_light = vec3(0);
  
  for(int i=0; i<sphere_arealights.num; ++i)
  {
    SphereAreaLight light = sphere_arealights.lights[i];
    
    Sphere sphere;
    sphere.origin = light.origin;
    sphere.radius = light.radius;
    
    LightInput light;
    
    light.lightSource = light.light;
    light.direction_to_light = getDirectionToLight(sphere);
    light.illuminance = sphereLightIlluminance(worldNormal, worldPosition, sphere);
    
    outgoing_light += do_the_lighting(material, viewer, light);
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
    
    LightInput light;
    
    light.lightSource = light.light;
    shading_input.direction_to_light = getDirectionToLight(rect);
    shading_input.illuminance = rectAreaLightIlluminance(worldPosition, worldNormal, rect);
    
    outgoing_light += do_the_lighting(material, viewer, light);
  }
  
  return outgoing_light + material.emission;
}


vec3 light_material(in MaterialInput material, in ViewerInput viewer)
{  
  return rendering_equation(material, viewer);
}
