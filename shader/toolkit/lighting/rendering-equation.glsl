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

