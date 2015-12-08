#ifdef FORWARD_RENDERER

#include <lighting/light-buffers.glsl>
#include "shading.glsl"
#include "common-uniform-blocks.glsl"

vec3 calc_debug_lighting(vec3 world_pos, vec3 world_normal)
{
  int N = 4;
  vec3 directions[4];
  vec3 colors[4];
  
  directions[0] = vec3(-0.76, -0.27, -0.59);
  directions[1] = vec3(0, 0,-1);
  directions[2] = vec3(0, 0, 1);
  directions[3] = vec3(0.76, 0.63, 0.16);
  
  colors[0] = vec3(1, 0.7, 0.3);
  colors[1] = vec3(0.2, 0.3, 1.0)*0.5;
  colors[2] = vec3(1, 0.7, 0.5)*0.3;
  colors[3] = vec3(1)*0.3;
  
  vec3 result = vec3(0);
  for(int i=0; i<N; ++i)
    result += colors[i] * max(0, dot(world_normal, -directions[i]));
    return result;
}


ShadingOutput direct_lighting(in ShadingInput shading_input)
{
  ShadingOutput shading_output;
  shading_output.diffuse_light = calc_debug_lighting(shading_input.surface_position, shading_input.surface_normal);
  shading_output.specular_light = vec3(0);
  
  return shading_output;
}


void apply_material(in MaterialOutput material_output)
{
  ShadingInput shading_input;
  shading_input.surface_normal = material_output.normal;
  shading_input.surface_roughness = material_output.roughness;
  shading_input.surface_position = material_output.position;
  shading_input.direction_to_viewer = scene.view_position;
  
  float alpha = material_output.color.a;
  vec3 emission = material_output.emission;
  vec3 diffuse_color = material_output.color.rgb;
  vec3 specular_color = mix(vec3(1), material_output.color.rgb, material_output.metallic);
  float specularfactor_dielectric = 0.05f;
  float specularfactor_metal = 1.f;
  float specularfactor = mix(specularfactor_dielectric, specularfactor_metal, material_output.metallic);
  
  ShadingOutput shading_output = direct_lighting(shading_input);
  
  vec3 lit_color = mix(diffuse_color*shading_output.diffuse_light, specular_color*shading_output.specular_light, specularfactor);
  
  color = vec4(emission + lit_color, alpha);
}

#endif // FORWARD_RENDERER
