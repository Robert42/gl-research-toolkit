#include <lighting/light-buffers.glsl>

// assumed invariant when using this struct:  (diffuse_color + specular_color) <= 1
struct ShadingInput
{
  vec3 surface_normal;
  float surface_roughness;
  vec3 surface_position;
  vec3 direction_to_viewer;
  vec3 diffuse_color;
  vec3 specular_color;
  vec3 emission;
};

/*
The following blinn_phong_brdf and lambertian_brdf is taken from
@book{RTR3,
 author = {Tomas Akenine-M\"{o}ller and Eric Haines and Naty Hoffman},
 title = {Real-Time Rendering 3rd Edition},
 year = {2008},
 pages = {1045},
 isbn = {987-1-56881-424-7},
 publisher = {A. K. Peters, Ltd.},
 address = {Natick, MA, USA},
 }

Equation 7.4.8  (page 257)

*/
vec3 blinn_phong_brdf(in ShadingInput shading_input, in vec3 direction_to_light)
{
  vec3 specular_color = shading_input.specular_color;
  vec3 half_vector = normalize(direction_to_light + shading_input.direction_to_viewer);
  float cos_theta_half_vector = max(0, dot(shading_input.surface_normal, half_vector));
  
  float roughness = shading_input.surface_roughness;
  
  float m = mix(1024.f, 1.f, roughness);
  
  return specular_color * pow(cos_theta_half_vector, m) * (m + 8.f) / (8.f * pi);
  
  // TODO: use instead equation 7.49 (fresnel is described in 7.5.3)
}

vec3 lambertian_brdf(in ShadingInput shading_input, in vec3 direction_to_light)
{
  vec3 diffuse_color = shading_input.diffuse_color;
  
  return diffuse_color / pi;
}

vec3 brdf(in ShadingInput shading_input, in vec3 direction_to_light)
{
  vec3 diffuse_term = lambertian_brdf(shading_input, direction_to_light);
  vec3 specular_term = blinn_phong_brdf(shading_input, direction_to_light);
  
  
  // Just adding them is ok, because of the invariant (diffuse_color + specular_color) <= 1
  return diffuse_term + specular_term;
}

vec3 do_the_lighting(in ShadingInput shading_input, in LightSource light, in vec3 light_position)
{
  vec3 luminance = light.luminance * light.color;
  float distance_to_light = length(light_position-shading_input.surface_position);
  vec3 direction_to_light = (light_position-shading_input.surface_position) / distance_to_light;
  float cos_factor = max(0, dot(direction_to_light, shading_input.surface_normal));
  
  float falloff = 1.f/sq(distance_to_light);
  falloff = clamp(falloff, 0, 1); // TODO decide whether to clamp of not
  
  return luminance * falloff * brdf(shading_input, direction_to_light) * cos_factor;
}


vec3 rendering_equation(in ShadingInput shading_input)
{
  vec3 outgoing_light = vec3(0);
  
  for(int i=0; i<sphere_arealights.num; ++i)
  {
    SphereAreaLight light = sphere_arealights.lights[i];
    
    outgoing_light += do_the_lighting(shading_input, light.light, light.origin);
  }
  
  for(int i=0; i<rect_arealights.num; ++i)
  {
    RectAreaLight light = rect_arealights.lights[i];
    
    outgoing_light += do_the_lighting(shading_input, light.light, light.origin);
  }
  
  return outgoing_light + shading_input.emission;
}


vec3 light_material(in MaterialOutput material_output, in vec3 direction_to_camera)
{
  float specularfactor_dielectric = 0.05f;
  float specularfactor_metal = 1.f;
  float specularfactor = mix(specularfactor_dielectric, specularfactor_metal, material_output.metallic);
  
  vec3 emission = material_output.emission;
  vec3 diffuse_color = material_output.color.rgb;
  vec3 specular_color = mix(vec3(1), material_output.color.rgb, material_output.metallic);
  
  // Ensure the invariant (diffuse_color + specular_color) <= 1
  diffuse_color *= 1.f - specularfactor;
  specular_color *= specularfactor;
  
  ShadingInput shading_input;
  shading_input.surface_normal = material_output.normal;
  shading_input.surface_roughness = material_output.roughness;
  shading_input.surface_position = material_output.position;
  shading_input.direction_to_viewer = direction_to_camera;
  shading_input.diffuse_color = diffuse_color;
  shading_input.specular_color = specular_color;
  shading_input.emission = emission;
  
  return rendering_equation(shading_input);
}
