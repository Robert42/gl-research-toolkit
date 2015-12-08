#include <lighting/light-buffers.glsl>

void phong_shading(in ShadingInput shading_input, in LightSource light, in vec3 light_position, inout vec3 diffuse_light, inout vec3 specular_light)
{
  vec3 luminance = light.luminance * light.color;
  
  vec3 light_direction = normalize(light_position-shading_input.surface_position);
  
  float factor = max(0, dot(light_direction, shading_input.surface_normal));
  
  diffuse_light += luminance * factor / pi;
  //specular_light += luminance * factor * () / pi;
}


ShadingOutput direct_lighting(in ShadingInput shading_input)
{
  ShadingOutput shading_output;
  shading_output.diffuse_light = vec3(0);
  shading_output.specular_light = vec3(0);
  
  for(int i=0; i<sphere_arealights.num; ++i)
  {
    SphereAreaLight light = sphere_arealights.lights[i];
    
    phong_shading(shading_input, light.light, light.origin, shading_output.diffuse_light, shading_output.specular_light);
  }
  
  for(int i=0; i<rect_arealights.num; ++i)
  {
    RectAreaLight light = rect_arealights.lights[i];
    
    phong_shading(shading_input, light.light, light.origin, shading_output.diffuse_light, shading_output.specular_light);
  }
  
  return shading_output;
}


vec3 light_material(in MaterialOutput material_output, in vec3 direction_to_camera)
{
  ShadingInput shading_input;
  shading_input.surface_normal = material_output.normal;
  shading_input.surface_roughness = material_output.roughness;
  shading_input.surface_position = material_output.position;
  shading_input.direction_to_viewer = direction_to_camera;
  
  float alpha = material_output.color.a;
  vec3 emission = material_output.emission;
  vec3 diffuse_color = material_output.color.rgb;
  vec3 specular_color = mix(vec3(1), material_output.color.rgb, material_output.metallic);
  float specularfactor_dielectric = 0.05f;
  float specularfactor_metal = 1.f;
  float specularfactor = mix(specularfactor_dielectric, specularfactor_metal, material_output.metallic);
  
  ShadingOutput shading_output = direct_lighting(shading_input);
  
  vec3 lit_color = mix(diffuse_color*shading_output.diffuse_light, specular_color*shading_output.specular_light, specularfactor);
  
  return emission + lit_color;
}
